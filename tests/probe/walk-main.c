/* The walk probe's main, compiled by clang -O0: calls every function of
   walk.c as rcc compiled it and as clang did (ref_), over a table of
   values, and compares.  Prints the first forty differences, or every
   one when PROBE_ALL is set in the environment, and a summary; exits 0
   when nothing differs, 1 otherwise.

   Memory the functions read and write is the main's, as in the
   pointers probe: each build has a buffer and eight cells of its own,
   filled alike before each check; B names the middle of the buffer of
   the build called, C its cells.  A check compares the results, then
   the bytes of the buffer, the guard bytes on both sides of what any
   function reaches included, then the cells as offsets from B.

   names_<n> is compared with clang's build nowhere: each of its
   assignments has a right operand that names the stepped pointer,
   which clang -O0 evaluates before the left operand's step, where G13
   evaluates the left operand first.  rcc's result and memory are
   compared with left_first_names_<n>, below, which makes the same
   accesses in steps the language sequences, the left operand's
   address and its step first; the line printed says "clang" of that
   function's result. */

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "walk.h"

static const long values[] = {
    0, 1, 2, 3, 5, 7, 85, 100, 127, 128, 200, 255, 256, 4095, 4096, 4097,
    5000, 32767, 32768, 65535, 65536, 0x12345, 0xfff000, 0x12345678,
    0x7fffffff, 0x80000000L, 0xffffffffL, 0x100000000L, 0x123456789abcdef0L,
    -1, -2, -3, -7, -128, -129, -4096, -5000, -32768, -32769, -65536,
    -2147483647L - 1, -2147483649L, LONG_MAX, LONG_MIN
};

#define COUNT ((int) (sizeof values / sizeof *values))

/* the bytes on each side of B a function reaches, *(p += 300) on a
   char and a little more; the guard beyond it */
#define SPAN 384
#define GUARD 32
#define CELLS 8

static unsigned char area[2][GUARD + 2 * SPAN + GUARD]
    __attribute__((aligned(16)));
static void *cell[2][CELLS];
static unsigned long seed;

static long total;
static long bad;
static long shown = 40;

static unsigned char *
base(int k)
{
    return area[k] + GUARD + SPAN;
}

/* both buffers filled with the same bytes, which change from check to
   check; the cells pointing into the buffer, alike */
static void
prime(void)
{
    ++seed;

    for (int i = -SPAN - GUARD; i < SPAN + GUARD; ++i)
        base(0)[i] = base(1)[i] =
            (unsigned char) (seed * 0x9e3779b1UL + i * 151UL + (i >> 3));

    for (int k = 0; k < 2; ++k)
        for (int c = 0; c < CELLS; ++c)
            cell[k][c] = base(k) - 200 + 48 * c + c % 3;
}

/* a pointer as its offset from the base of its build's buffer */
static long
offset(const void *p, const unsigned char *b)
{
    return (long) ((unsigned long) p - (unsigned long) b);
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

/* the buffers byte by byte, guards included, and the cells */
static void
compare(const char *name, long x, long y)
{
    ++total;

    for (int i = -SPAN - GUARD; i < SPAN + GUARD; ++i) {
        if (base(0)[i] == base(1)[i])
            continue;

        if (++bad <= shown)
            printf("%s(%ld, %ld, ...): byte B%+d: rcc %d, clang %d\n", name,
                   x, y, i, base(0)[i], base(1)[i]);
        return;
    }

    for (int c = 0; c < CELLS; ++c) {
        const long got = offset(cell[0][c], base(0));
        const long want = offset(cell[1][c], base(1));

        if (got == want)
            continue;

        if (++bad <= shown)
            printf("%s(%ld, %ld, ...): cell %d: rcc B%+ld, clang B%+ld\n",
                   name, x, y, c, got, want);
        return;
    }
}

/* f and its reference on the same arguments, no memory of the main's */
#define CHECK(f, x, y, arguments)                                       \
    report(#f, (long) (x), (long) (y), (long) f arguments,              \
           (long) ref_##f arguments)

/* the result of a call, as a long or as nothing for a void function */
#define AS_LONG(e) ((long) (e))
#define AS_VOID(e) ((e), 0L)

/* f and the reference ref on the same arguments, each with its own
   buffer and cells, filled alike; the results compared, then the
   memory */
#define SIDES(as, f, ref, x, y, arguments)                              \
    do {                                                                \
        long got_;                                                      \
        long want_;                                                     \
                                                                        \
        prime();                                                        \
        {                                                               \
            unsigned char *const B = base(0);                           \
            void **const C = cell[0];                                   \
            got_ = as(f arguments);                                     \
        }                                                               \
        {                                                               \
            unsigned char *const B = base(1);                           \
            void **const C = cell[1];                                   \
            want_ = as(ref arguments);                                  \
        }                                                               \
        report(#f, (long) (x), (long) (y), got_, want_);                \
        compare(#f, (long) (x), (long) (y));                            \
    } while (0)

#define MCHECK(f, x, y, arguments)                                      \
    SIDES(AS_LONG, f, ref_##f, x, y, arguments)
#define VCHECK(f, x, y, arguments)                                      \
    SIDES(AS_VOID, f, ref_##f, x, y, arguments)

/* f against left_first_f, G13's order, in place of clang's build */
#define LCHECK(f, x, y, arguments)                                      \
    SIDES(AS_LONG, f, left_first_##f, x, y, arguments)

/* declarations: each function and its reference */
#define BOTH(R, f, parameters) R f parameters; R ref_##f parameters;

#define DECLARE_POINTEE(T, N)                                           \
    BOTH(long, load_##N, (void *))                                      \
    BOTH(long, store_##N, (void *, long, long))                         \
    BOTH(long, ends_##N, (void *)) BOTH(long, names_##N, (void *))      \
    BOTH(long, rmw_##N, (void *, long))                                 \
    BOTH(long, held_##N, (void *, int))                                 \
    BOTH(long, discard_##N, (void *, int))                              \
    BOTH(long, other_##N, (void *, long))

POINTEES(DECLARE_POINTEE)

#define DECLARE_JUMP(T, N, c) BOTH(long, jump_##N##_##c, (void *, long))
#define DECLARE_JUMPS(T, N) NEAR(DECLARE_JUMP, T, N)

POINTEES(DECLARE_JUMPS)
FAR(DECLARE_JUMP)

BOTH(long, twice, (long))
BOTH(long, wcopy, (void *, char *))
BOTH(long, wlength, (char *))
BOTH(int, wcompare, (char *, char *))
BOTH(void, wreverse, (void *, long))
BOTH(long, cells, (void *))
BOTH(long, row, (void *, long))

/* walk-lib.c */
long lid(long a);

/* names_<n> of walk.c by G13: each assignment's left operand first,
   its address and its step, then the right operand, then the store,
   or the load, the operation and the store of a compound one; the
   value of an assignment is what it stored, read back. */
#define LEFT_FIRST(T, N)                                                \
    static long left_first_names_##N(void *b)                           \
    {                                                                   \
        T *const base = b;                                              \
        T *p = base;                                                    \
        T *a;                                                           \
        T *c;                                                           \
        long r;                                                         \
                                                                        \
        a = p++;                                                        \
        *a = *p;                                                        \
        a = p++;                                                        \
        *a = (T) (p - base);                                            \
        a = ++p;                                                        \
        *a = *p;                                                        \
        a = p++;                                                        \
        c = p++;                                                        \
        *a = *c;                                                        \
        a = p--;                                                        \
        *a = (T) lid((long) p - (long) base);                           \
        a = --p;                                                        \
        *a = (T) ((long) p - (long) base);                              \
        a = p++;                                                        \
        *a += (long) *p;                                                \
        r = p - base;                                                   \
        a = p++;                                                        \
        *a = *p;                                                        \
        r = r * 31 + (long) *a;                                         \
        a = ++p;                                                        \
        *a = (T) (p - base);                                            \
        r = r * 31 + (long) *a;                                         \
        a = p--;                                                        \
        c = p--;                                                        \
        *a = *c;                                                        \
        r = r * 31 + (long) *a;                                         \
        a = p--;                                                        \
        *a -= (long) *p;                                                \
        r = r * 31 + (long) *a;                                         \
                                                                        \
        return r * 31 + (p - base);                                     \
    }

POINTEES(LEFT_FIRST)

/* a pointer the main made at B + at, of any pointee */
#define P ((void *) (B + at))

/* the functions of one pointee over the values, on B and up to 7
   bytes past it, so that a wide pointee is also walked misaligned */
#define TEST_JUMP(T, N, c) MCHECK(jump_##N##_##c, x, at, (P, x));
#define TEST_POINTEE(T, N)                                              \
    static void test_##N(void)                                          \
    {                                                                   \
        for (int i = 0; i < COUNT; ++i) {                               \
            const long x = values[i];                                   \
            const long y = values[(i * 7 + 3) % COUNT];                 \
            const int at = i % 8;                                       \
                                                                        \
            MCHECK(load_##N, at, 0, (P));                               \
            MCHECK(store_##N, x, y, (P, x, y));                         \
            MCHECK(ends_##N, at, 0, (P));                               \
            LCHECK(names_##N, at, 0, (P));                              \
            MCHECK(rmw_##N, x, at, (P, x));                             \
            MCHECK(held_##N, at, 0, (P, 0));                            \
            MCHECK(held_##N, at, 1, (P, 1));                            \
            MCHECK(discard_##N, at, i % 21, (P, i % 21));               \
            MCHECK(other_##N, x, at, (P, x));                           \
            NEAR(TEST_JUMP, T, N)                                       \
        }                                                               \
    }

POINTEES(TEST_POINTEE)

/* the constants past the form's reach, and the walks through the cells
   and through a row */
static void
test_far(void)
{
    for (int i = 0; i < COUNT; ++i) {
        const long x = values[i];
        const int at = i % 8;

        FAR(TEST_JUMP)
        MCHECK(cells, i, 0, ((void *) C));
        MCHECK(row, x, at, (P, x));
    }
}

/* the string idioms over strings of each kind, the reverse over every
   length to 20 */
static void
test_strings(void)
{
    static char *strings[] = {
        "", "a", "walked to its terminator", "\001\177\200\377xyz",
        "walked", "walked to", "\200", "walked\377"
    };
    const int strings_count = (int) (sizeof strings / sizeof *strings);

    for (int i = 0; i < strings_count; ++i) {
        CHECK(wlength, i, 0, (strings[i]));
        MCHECK(wcopy, i, 0, (B - 40, strings[i]));

        for (int j = 0; j < strings_count; ++j)
            CHECK(wcompare, i, j, (strings[i], strings[j]));
    }

    for (int n = 0; n < 21; ++n)
        for (int at = 0; at < 8; at += 3)
            VCHECK(wreverse, n, at, (P, n));
}

int
main(void)
{
    if (getenv("PROBE_ALL"))
        shown = LONG_MAX;

    for (int i = 0; i < COUNT; ++i)
        CHECK(twice, values[i], 0, (values[i]));

#define RUN(T, N) test_##N();
    POINTEES(RUN)

    test_far();
    test_strings();

    printf("%ld checks, %ld differ\n", total, bad);

    return bad != 0;
}
