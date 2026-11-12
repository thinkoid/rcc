/* The registers probe's main, compiled by clang -O0: calls every
   function of registers.c as rcc compiled it and as clang did (ref_),
   over a table of values, and compares.  Prints the first forty
   differences, or every one when PROBE_ALL is set in the environment,
   and a summary; exits 0 when nothing differs, 1 otherwise.

   A function of registers.c whose parameters are narrow is declared
   here with long ones, and is passed whole longs: the bits above the
   parameter's width are whatever the value has there, which the
   convention allows a caller and the callee must not read.  That is
   the test of a narrow register parameter.

   Memory the pointer functions read and write is the main's: each
   build has a buffer of its own, filled alike before each check; B
   names the middle of the buffer of the build called.  A check
   compares the results, then the bytes of both buffers. */

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "registers.h"

static const long values[] = {
    0, 1, 2, 3, 5, 7, 85, 100, 127, 128, 200, 255, 256, 4095, 4096, 4097,
    5000, 32767, 32768, 65535, 65536, 0x12345, 0xfff000, 0x12345678,
    0x7fffffff, 0x80000000L, 0xffffffffL, 0x100000000L, 0x123456789abcdef0L,
    -1, -2, -3, -7, -128, -129, -4096, -5000, -32768, -32769, -65536,
    -2147483647L - 1, -2147483649L, LONG_MAX, LONG_MIN
};

#define COUNT ((int) (sizeof values / sizeof *values))

/* the indexes of a subscript, so that p[k + m] stays within REACH
   elements of B */
static const long indexes[] = {
    0, 1, 2, 3, 7, 15, 31, 63, -1, -2, -8, -63
};

#define INDEX_COUNT ((int) (sizeof indexes / sizeof *indexes))
#define REACH 130

/* the bytes on each side of B, and the buffers */
#define WIDEST (REACH * 8)

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

#define DECLARE_BINARY(T, N, name, op, small, large)                    \
    BOTH(long, name##_##N, (long, long))
#define DECLARE_SHIFT(T, N, name, op) BOTH(long, name##_##N, (long, long))

#define DECLARE_TYPE(T, N)                                              \
    BOTH(long, init_##N, (long, long)) BOTH(long, kinit_##N, (void))    \
    BOTH(long, kinit2_##N, (void))                                      \
    OPERATORS(DECLARE_BINARY, T, N) DIVISIONS(DECLARE_BINARY, T, N)     \
    SHIFTS(DECLARE_SHIFT, T, N) BOTH(long, unary_##N, (long))           \
    BOTH(long, assign_##N, (long, long, long))                          \
    BOTH(long, assign_order_##N, (long, long))                          \
    BOTH(long, cond_##N, (long, long))                                  \
    BOTH(long, held_##N, (long, long, int))                             \
    BOTH(long, across_##N, (long, long)) BOTH(long, result_##N, (long)) \
    BOTH(long, call_order_##N, (long)) BOTH(long, own_order_##N, (long)) \
    BOTH(long, rpar1_##N, (long)) BOTH(long, rfirst_##N, (P12(long)))   \
    BOTH(long, rmiddle_##N, (P12(long))) BOTH(long, rlast_##N, (P12(long))) \
    BOTH(long, rall_##N, (P12(long)))                                   \
    BOTH(long, rwritten_##N, (long, long, long))                        \
    BOTH(long, rcall_##N, (long, long))

TYPES(DECLARE_TYPE)

#define DECLARE_CAST(F, FN, T, TN) BOTH(long, cast_##FN##_##TN, (long))
#define DECLARE_CASTS(F, FN) TYPES2(DECLARE_CAST, F, FN)

TYPES(DECLARE_CASTS)

#define DECLARE_POINTER(E, T)                                           \
    BOTH(long, pread_##E, (void *, long, int))                          \
    BOTH(void, pwrite_##E, (void *, long, int, long))                   \
    BOTH(long, parith_##E, (void *, long, int))

SCALARS(DECLARE_POINTER)

#define DECLARE_UNWRITTEN(T, N)                                         \
    BOTH(long, unwritten_##N, (void)) BOTH(long, fill_##N, (long))

CLAIMED(DECLARE_UNWRITTEN)

BOTH(long, twice, (long))
BOTH(long, rsum, (int, long))
BOTH(int, rfib, (int))
BOTH(long, clobber1, (long))
BOTH(long, clobber2, (long))
BOTH(long, clobber3, (long))
BOTH(long, clobber4, (long))
BOTH(long, clobber5, (long))
BOTH(long, clobber6, (long))
BOTH(long, clobber7, (long))
BOTH(long, clobber8, (long))
BOTH(long, clobber9, (long))
BOTH(long, clobber10, (long))
BOTH(long, keep1, (long))
BOTH(long, keep2, (long))
BOTH(long, keep3, (long))
BOTH(long, keep4, (long))
BOTH(long, keep5, (long))
BOTH(long, keep6, (long))
BOTH(long, keep7, (long))
BOTH(long, keep8, (long))
BOTH(long, keep9, (long))
BOTH(long, keep10, (long))
BOTH(long, blocks, (long, long))
BOTH(long, eleven, (long, long))
BOTH(long, pwalk, (char *))
BOTH(long, puwalk, (unsigned char *, int))
BOTH(long, frame_far, (long, long))
BOTH(long, deep_reg, (P8(long)))
BOTH(long, deep_reg_call, (P8(long)))

/* twelve values of the table, a different walk for each i and k */
static void
fill(long v[12], int i, int k)
{
    for (int p = 0; p < 12; ++p)
        v[p] = values[(i + p * k) % COUNT];
}

#define V8 v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7]
#define V12 V8, v[8], v[9], v[10], v[11]

/* A divisor of 0 or -1 is skipped: the first has no value in C and the
   second none for the least dividend, and what the instruction does
   then is G4's, not this probe's.  The division functions divide by
   both their operands. */
#define DIVIDES(T, x) ((T) (x) != 0 && (T) (x) != (T) -1)

#define TEST_BINARY(T, N, name, op, small, large)                       \
    CHECK(name##_##N, x, y, (x, y));
#define TEST_DIVISION(T, N, name, op, small, large)                     \
    if (DIVIDES(T, x) && DIVIDES(T, y))                                 \
        CHECK(name##_##N, x, y, (x, y));

/* the functions of one type over pairs of values; the shifts over
   every count below 32, which is below the width of every promoted
   type */
#define TEST_TYPE(T, N)                                                 \
    static void test_##N(void)                                          \
    {                                                                   \
        CHECK(kinit_##N, 0, 0, ());                                     \
        CHECK(kinit2_##N, 0, 0, ());                                    \
                                                                        \
        for (int i = 0; i < COUNT; ++i) {                               \
            const long x = values[i];                                   \
                                                                        \
            CHECK(unary_##N, x, 0, (x));                                \
            CHECK(result_##N, x, 0, (x));                               \
            CHECK(call_order_##N, x, 0, (x));                           \
            CHECK(own_order_##N, x, 0, (x));                            \
            CHECK(rpar1_##N, x, 0, (x));                                \
                                                                        \
            for (int n = 0; n < 32; ++n) {                              \
                CHECK(shl_##N, x, n, (x, n));                           \
                CHECK(shr_##N, x, n, (x, n));                           \
            }                                                           \
                                                                        \
            for (int j = 0; j < COUNT; ++j) {                           \
                const long y = values[j];                               \
                const long w = values[(i * 7 + j) % COUNT];             \
                                                                        \
                CHECK(init_##N, x, y, (x, y));                          \
                OPERATORS(TEST_BINARY, T, N)                            \
                DIVISIONS(TEST_DIVISION, T, N)                          \
                CHECK(assign_##N, x, y, (x, y, w));                     \
                CHECK(assign_order_##N, x, y, (x, y));                  \
                CHECK(cond_##N, x, y, (x, y));                          \
                CHECK(held_##N, x, y, (x, y, 0));                       \
                CHECK(held_##N, x, y, (x, y, 1));                       \
                CHECK(across_##N, x, y, (x, y));                        \
                CHECK(rwritten_##N, x, y, (x, y, w));                   \
                CHECK(rcall_##N, x, y, (x, y));                         \
            }                                                           \
                                                                        \
            for (int k = 1; k < COUNT; k += 3) {                        \
                long v[12];                                             \
                                                                        \
                fill(v, i, k);                                          \
                                                                        \
                CHECK(rfirst_##N, v[0], v[1], (V12));                   \
                CHECK(rmiddle_##N, v[0], v[1], (V12));                  \
                CHECK(rlast_##N, v[0], v[1], (V12));                    \
                CHECK(rall_##N, v[0], v[1], (V12));                     \
            }                                                           \
        }                                                               \
    }

TYPES(TEST_TYPE)

#define TEST_CAST(F, FN, T, TN) CHECK(cast_##FN##_##TN, x, 0, (x));
#define TEST_CASTS(F, FN) TYPES2(TEST_CAST, F, FN)

static void
test_casts(void)
{
    for (int i = 0; i < COUNT; ++i) {
        const long x = values[i];

        TYPES(TEST_CASTS)
    }
}

/* the subscripts at every pair of indexes, on B and on B + 5; the
   arithmetic, which reads no memory, on B */
#define TEST_POINTER(E, T)                                              \
    static void test_pointer_##E(void)                                  \
    {                                                                   \
        for (int i = 0; i < INDEX_COUNT; ++i) {                         \
            for (int j = 0; j < INDEX_COUNT; ++j) {                     \
                const long k = indexes[i];                              \
                const int m = (int) indexes[j];                         \
                const long v = values[(i * 5 + j) % COUNT];             \
                                                                        \
                MCHECK(pread_##E, k, m, (B, k, m));                     \
                MCHECK(pread_##E, k, m, (B + 5, k, m));                 \
                VCHECK(pwrite_##E, k, m, (B, k, m, v));                 \
                VCHECK(pwrite_##E, k, m, (B + 5, k, m, v));             \
                MCHECK(parith_##E, k, m, (B, k, m));                    \
            }                                                           \
        }                                                               \
    }

SCALARS(TEST_POINTER)

/* G5: the fills include a byte, a half and a word whose sign bit is
   set, and patterns whose bits above any narrow width are not the
   extension of the bits below */
static const long fills[] = {
    -1, 0, 0x5a5a5a5a5a5a5a5aL, 0x80, 0x8000, 0x80000000L,
    0x123456789abcdef0L, 0x7f7f7f7f7f7f7f7fL, -0x7f7f7f7f7f7f7f80L
};

#define FILL_COUNT ((int) (sizeof fills / sizeof *fills))

#define TEST_UNWRITTEN(T, N)                                            \
    CHECK(unwritten_##N, 0, 0, ());                                     \
    for (int i = 0; i < FILL_COUNT; ++i)                                \
        CHECK(fill_##N, fills[i], 0, (fills[i]));                       \
    for (int i = 0; i < COUNT; ++i)                                     \
        CHECK(fill_##N, values[i], 0, (values[i]));

static void
test_rest(void)
{
    static char *strings[] = {
        "", "a", "walked to its terminator", "\001\177\200\377xyz"
    };

    for (int i = 0; i < 4; ++i)
        CHECK(pwalk, i, 0, (strings[i]));

    for (int n = 0; n < 8; ++n)
        CHECK(puwalk, n, 0, ((unsigned char *) strings[3], n));

    for (int i = 0; i < COUNT; ++i) {
        const long x = values[i];

        CHECK(twice, x, 0, (x));
        CHECK(clobber1, x, 0, (x));
        CHECK(clobber10, x, 0, (x));
        CHECK(keep1, x, 0, (x));
        CHECK(keep2, x, 0, (x));
        CHECK(keep3, x, 0, (x));
        CHECK(keep4, x, 0, (x));
        CHECK(keep5, x, 0, (x));
        CHECK(keep6, x, 0, (x));
        CHECK(keep7, x, 0, (x));
        CHECK(keep8, x, 0, (x));
        CHECK(keep9, x, 0, (x));
        CHECK(keep10, x, 0, (x));

        for (int j = 0; j < COUNT; ++j) {
            const long y = values[j];

            CHECK(blocks, x, y, (x, y));
            CHECK(eleven, x, y, (x, y));
            CHECK(frame_far, x, y, (x, y));
        }

        for (int k = 1; k < COUNT; k += 3) {
            long v[12];

            fill(v, i, k);

            CHECK(deep_reg, v[0], v[1], (V8));
            CHECK(deep_reg_call, v[0], v[1], (V8));
        }
    }

    for (int n = -2; n < 40; ++n)
        for (int i = 0; i < COUNT; i += 5)
            CHECK(rsum, n, values[i], (n, values[i]));

    for (int n = 0; n < 21; ++n)
        CHECK(rfib, n, 0, (n));

    CLAIMED(TEST_UNWRITTEN)
}

int
main(void)
{
    if (getenv("PROBE_ALL"))
        shown = LONG_MAX;

#define RUN(T, N) test_##N();
    TYPES(RUN)

    test_casts();

#define RUN_POINTER(E, T) test_pointer_##E();
    SCALARS(RUN_POINTER)

    test_rest();

    printf("%ld checks, %ld differ\n", total, bad);

    return bad != 0;
}
