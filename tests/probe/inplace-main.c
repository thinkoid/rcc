/* The inplace probe's main, compiled by clang -O0: calls every
   function of inplace.c as rcc compiled it and as clang did (ref_),
   over a table of values, and compares.  Prints the first forty
   differences, or every one when PROBE_ALL is set in the environment,
   and a summary; exits 0 when nothing differs, 1 otherwise.

   A function of inplace.c whose parameters are narrow is declared
   here with long ones, and is passed whole longs: the bits above the
   parameter's width are whatever the value has there, which the
   convention allows a caller and the callee must not read.

   Memory the pointer functions read and write is the main's: each
   build has a buffer of its own, filled alike before each check; B
   names the middle of the buffer of the build called.  A check
   compares the results, then the bytes of both buffers. */

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "inplace.h"

static const long values[] = {
    0, 1, 2, 3, 5, 7, 85, 100, 127, 128, 200, 255, 256, 4095, 4096, 4097,
    5000, 32767, 32768, 65535, 65536, 0x12345, 0xfff000, 0x12345678,
    0x7fffffff, 0x80000000L, 0xffffffffL, 0x100000000L, 0x123456789abcdef0L,
    -1, -2, -3, -7, -128, -129, -4096, -5000, -32768, -32769, -65536,
    -2147483647L - 1, -2147483649L, LONG_MAX, LONG_MIN
};

#define COUNT ((int) (sizeof values / sizeof *values))

/* the bytes on each side of B, and the buffers */
#define WIDEST 512

static unsigned char area[2][2 * WIDEST] __attribute__((aligned(16)));
static unsigned long seed;

static long total;
static long bad;
static long shown = 40;

static unsigned char *
base(int k)
{
    return area[k] + WIDEST;
}

/* both buffers filled with the same bytes, which change from check to
   check */
static void
prime(void)
{
    ++seed;

    for (int i = -WIDEST; i < WIDEST; ++i)
        base(0)[i] = base(1)[i] =
            (unsigned char) (seed * 0x9e3779b1UL + i * 151UL + (i >> 3));
}

static void
report(const char *name, long x, long y, long got, long want)
{
    ++total;

    if (got == want)
        return;

    if (++bad <= shown)
        printf("%s(%ld, %ld, ...): rcc %ld, clang %ld\n", name, x, y, got,
               want);
}

/* the buffers byte by byte */
static void
compare(const char *name, long x, long y)
{
    ++total;

    for (int i = -WIDEST; i < WIDEST; ++i) {
        if (base(0)[i] == base(1)[i])
            continue;

        if (++bad <= shown)
            printf("%s(%ld, %ld, ...): byte B%+d: rcc %d, clang %d\n", name,
                   x, y, i, base(0)[i], base(1)[i]);
        return;
    }
}

/* f and its reference on the same arguments */
#define CHECK(f, x, y, arguments)                                       \
    report(#f, (long) (x), (long) (y), (long) f arguments,              \
           (long) ref_##f arguments)

/* the result of a call, as a long or as nothing for a void function */
#define AS_LONG(e) ((long) (e))
#define AS_VOID(e) ((e), 0L)

/* f and its reference on the same arguments, each with its own buffer,
   filled alike; the results compared, then the memory */
#define SIDES(as, f, x, y, arguments)                                   \
    do {                                                                \
        long got_;                                                      \
        long want_;                                                     \
                                                                        \
        prime();                                                        \
        {                                                               \
            unsigned char *const B = base(0);                           \
            got_ = as(f arguments);                                     \
        }                                                               \
        {                                                               \
            unsigned char *const B = base(1);                           \
            want_ = as(ref_##f arguments);                              \
        }                                                               \
        report(#f, (long) (x), (long) (y), got_, want_);                \
        compare(#f, (long) (x), (long) (y));                            \
    } while (0)

#define MCHECK(f, x, y, arguments) SIDES(AS_LONG, f, x, y, arguments)
#define VCHECK(f, x, y, arguments) SIDES(AS_VOID, f, x, y, arguments)

/* declarations: each function and its reference */
#define BOTH(R, f, parameters) R f parameters; R ref_##f parameters;

#define DECLARE_COMPOUND(T, N, name, op, small, large)                  \
    BOTH(long, name##_##N, (long, long, long))
#define DECLARE_SHIFT(T, N, name, op) BOTH(long, name##_##N, (long, long, long))

#define DECLARE_TYPE(T, N)                                              \
    COMPOUNDS(DECLARE_COMPOUND, T, N) DIVISIONS(DECLARE_COMPOUND, T, N) \
    SHIFTS(DECLARE_SHIFT, T, N)                                         \
    BOTH(long, step_##N, (long)) BOTH(long, steps_##N, (long, long))    \
    BOTH(long, ord_self_##N, (long)) BOTH(long, ord_twice_##N, (long))  \
    BOTH(long, ord_down_##N, (long)) BOTH(long, ord_add_##N, (long))    \
    BOTH(long, ord_own_##N, (long)) BOTH(long, ord_cond_##N, (long, int)) \
    BOTH(long, ord_and_##N, (long, int)) BOTH(long, ord_call_##N, (long)) \
    BOTH(long, ord_rcc_##N, (long)) BOTH(long, live_##N, (long, long))

TYPES(DECLARE_TYPE)

#define DECLARE_PSTEP(E, T, S) BOTH(long, pstep_##E, (void *, long))

POINTEES(DECLARE_PSTEP)

#define DECLARE_SCALAR(E, T)                                            \
    BOTH(long, walk_##E, (void *, long))                                \
    BOTH(long, index_##E, (void *, int, long))                          \
    BOTH(long, loops_##E, (void *, int)) BOTH(long, sum_##E, (void *, int))

SCALARS(DECLARE_SCALAR)

BOTH(long, twice, (long))
BOTH(long, widths, (long, long, long, long, long))
BOTH(long, widths_value, (long, long, long, long, long))
BOTH(long, rstrlen, (char *))
BOTH(long, rstrcpy, (void *, char *))
BOTH(void, rmemcpy, (void *, char *, long))
BOTH(int, rstrcmp, (char *, char *))
BOTH(long, own_steps, (long))
BOTH(long, rcount, (int, long))

/* A divisor of 0 or -1 is skipped: the first has no value in C and the
   second none for the least dividend, and what the instruction does
   then is G4's, not this probe's. */
#define DIVIDES(T, x) ((T) (x) != 0 && (T) (x) != (T) -1)

#define TEST_COMPOUND(T, N, name, op, small, large)                     \
    CHECK(name##_##N, x, y, (x, y, w));
#define TEST_DIVISION(T, N, name, op, small, large)                     \
    if (DIVIDES(T, y))                                                  \
        CHECK(name##_##N, x, y, (x, y, w));
#define TEST_SHIFT(T, N, name, op)                                      \
    for (int n = 0; n < 32; ++n)                                        \
        CHECK(name##_##N, x, n, (x, n, values[(i + n) % COUNT]));

/* the functions of one type over pairs of values; the shifts over
   every count below 32, which is below the width of every promoted
   type */
#define TEST_TYPE(T, N)                                                 \
    static void test_##N(void)                                          \
    {                                                                   \
        for (int i = 0; i < COUNT; ++i) {                               \
            const long x = values[i];                                   \
                                                                        \
            CHECK(step_##N, x, 0, (x));                                 \
            CHECK(ord_self_##N, x, 0, (x));                             \
            CHECK(ord_twice_##N, x, 0, (x));                            \
            CHECK(ord_down_##N, x, 0, (x));                             \
            CHECK(ord_add_##N, x, 0, (x));                              \
            CHECK(ord_own_##N, x, 0, (x));                              \
            CHECK(ord_cond_##N, x, 0, (x, 0));                          \
            CHECK(ord_cond_##N, x, 1, (x, 1));                          \
            CHECK(ord_and_##N, x, 0, (x, 0));                           \
            CHECK(ord_and_##N, x, 1, (x, 1));                           \
            CHECK(ord_call_##N, x, 0, (x));                             \
            CHECK(ord_rcc_##N, x, 0, (x));                              \
            SHIFTS(TEST_SHIFT, T, N)                                    \
                                                                        \
            for (int j = 0; j < COUNT; ++j) {                           \
                const long y = values[j];                               \
                const long w = values[(i * 7 + j) % COUNT];             \
                                                                        \
                COMPOUNDS(TEST_COMPOUND, T, N)                          \
                DIVISIONS(TEST_DIVISION, T, N)                          \
                CHECK(steps_##N, x, y, (x, y));                         \
                CHECK(live_##N, x, y, (x, y));                          \
            }                                                           \
        }                                                               \
    }

TYPES(TEST_TYPE)

/* the steps of a pointer by every value of the table, never read
   through */
#define TEST_PSTEP(E, T, S)                                             \
    for (int i = 0; i < COUNT; ++i)                                     \
        CHECK(pstep_##E, values[i], 0, (base(0), values[i]));

/* the walks on B and B + 3, the loops over every count to 20 */
#define TEST_SCALAR(E, T)                                               \
    for (int i = 0; i < COUNT; ++i) {                                   \
        MCHECK(walk_##E, values[i], 0, (B, values[i]));                 \
        MCHECK(walk_##E, values[i], 3, (B + 3, values[i]));             \
    }                                                                   \
    for (int n = 0; n < 16; ++n)                                        \
        MCHECK(index_##E, n, 0, (B, n, values[n * 2]));                 \
    for (int n = 0; n < 21; ++n) {                                      \
        MCHECK(loops_##E, n, 0, (B, n));                                \
        MCHECK(sum_##E, n, 0, (B - 8, n));                              \
    }

static void
test_pointers(void)
{
    static char *strings[] = {
        "", "a", "walked to its terminator", "\001\177\200\377xyz",
        "walked", "walked to"
    };
    const int strings_count = (int) (sizeof strings / sizeof *strings);

    POINTEES(TEST_PSTEP)

    SCALARS(TEST_SCALAR)

    for (int i = 0; i < strings_count; ++i) {
        CHECK(rstrlen, i, 0, (strings[i]));
        MCHECK(rstrcpy, i, 0, (B - 40, strings[i]));

        for (int n = 0; n < 8; ++n)
            VCHECK(rmemcpy, i, n, (B + n, strings[3], n));

        for (int j = 0; j < strings_count; ++j)
            CHECK(rstrcmp, i, j, (strings[i], strings[j]));
    }
}

/* mixed widths over pairs of values and a third, the divisor kept */
static void
test_rest(void)
{
    for (int i = 0; i < COUNT; ++i) {
        const long x = values[i];

        CHECK(twice, x, 0, (x));
        CHECK(own_steps, x, 0, (x));

        for (int j = 0; j < COUNT; ++j) {
            const long y = values[j];
            const long w = values[(i * 7 + j) % COUNT];
            const long z = values[(i + j * 3) % COUNT];

            if (!DIVIDES(long, w))
                continue;

            CHECK(widths, x, y, (x, y, w, z, x ^ y));
            CHECK(widths_value, x, y, (x, y, w, z, x ^ y));
        }
    }

    for (int n = -2; n < 40; ++n)
        for (int i = 0; i < COUNT; i += 5)
            CHECK(rcount, n, values[i], (n, values[i]));
}

int
main(void)
{
    if (getenv("PROBE_ALL"))
        shown = LONG_MAX;

#define RUN(T, N) test_##N();
    TYPES(RUN)

    test_pointers();
    test_rest();

    printf("%ld checks, %ld differ\n", total, bad);

    return bad != 0;
}
