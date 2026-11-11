/* The control probe's main, compiled by clang -O0: calls every
   function of control.c as rcc compiled it and as clang did (ref_),
   over a table of values, and compares.  Prints each difference and a
   summary; exits 0 when nothing differs, 1 otherwise. */

#include <limits.h>
#include <stdio.h>

#include "control.h"

static const long values[] = {
    0, 1, 2, 3, 5, 7, 85, 100, 127, 128, 200, 255, 256, 4095, 4096, 4097,
    5000, 32767, 32768, 65535, 65536, 0x12345, 0xfff000, 0x12345678,
    0x7fffffff, 0x80000000L, 0xffffffffL, 0x100000000L, 0x123456789abcdef0L,
    -1, -2, -3, -7, -128, -129, -4096, -5000, -32768, -32769, -65536,
    -2147483647L - 1, -2147483649L, LONG_MAX, LONG_MIN
};

#define COUNT ((int) (sizeof values / sizeof *values))

static long total;
static long bad;

static void
report(const char *name, long x, long y, long got, long want)
{
    ++total;

    if (got == want)
        return;

    if (++bad <= 40)
        printf("%s(%ld, %ld): rcc %ld, clang %ld\n", name, x, y, got, want);
}

/* f and its reference on the same arguments */
#define CHECK(f, x, y, arguments)                                       \
    report(#f, (long) (x), (long) (y), (long) f arguments,              \
           (long) ref_##f arguments)

/* declarations: each function and its reference */
#define BOTH(R, f, parameters) R f parameters; R ref_##f parameters;

#define DECLARE_TYPE(T, N)                                              \
    BOTH(int, lt_##N, (T, T)) BOTH(int, le_##N, (T, T))                 \
    BOTH(int, gt_##N, (T, T)) BOTH(int, ge_##N, (T, T))                 \
    BOTH(int, eq_##N, (T, T)) BOTH(int, ne_##N, (T, T))                 \
    BOTH(int, not_##N, (T)) BOTH(int, notnot_##N, (T))                  \
    BOTH(int, notlt_##N, (T, T))                                        \
    BOTH(int, and_##N, (T, T)) BOTH(int, or_##N, (T, T))                \
    BOTH(T, sel_##N, (T, T)) BOTH(T, min_##N, (T, T))                   \
    BOTH(int, iflt_##N, (T, T)) BOTH(int, ifle_##N, (T, T))             \
    BOTH(int, ifgt_##N, (T, T)) BOTH(int, ifge_##N, (T, T))             \
    BOTH(int, ifeq_##N, (T, T)) BOTH(int, ifne_##N, (T, T))             \
    BOTH(int, truth_##N, (T)) BOTH(int, false_##N, (T))                 \
    BOTH(int, both_##N, (T, T)) BOTH(int, either_##N, (T, T))           \
    BOTH(int, neither_##N, (T, T))                                      \
    BOTH(int, stale_##N, (long)) BOTH(int, stale_not_##N, (long, long)) \
    BOTH(int, stale_and_##N, (long, long))                              \
    BOTH(int, mixed_##N, (T, T)) BOTH(T, held_##N, (T, T))              \
    BOTH(T, arms_##N, (T, T))

TYPES(DECLARE_TYPE)

#define DECLARE_KCMP(T, N, name, op, constant)                          \
    BOTH(int, k##N##_##name, (T)) BOTH(int, j##N##_##name, (T))
#define DECLARE_KCMP_ALL(T, N) KCMPS(DECLARE_KCMP, T, N)

WIDE(DECLARE_KCMP_ALL)

BOTH(long, arms_mixed, (int, int, long))
BOTH(unsigned long, arms_unsigned, (int, unsigned, long))
BOTH(int, constant_conditions, (int))
BOTH(int, count, (int))
BOTH(int, sum, (int))
BOTH(int, last, (int))
BOTH(int, pairs, (int))
BOTH(long, nested, (int, int))
BOTH(int, gcd, (int, int))
BOTH(unsigned, collatz, (unsigned))
BOTH(int, first_bit, (unsigned))
BOTH(int, primes, (int))
BOTH(int, deep, (int, int, int, int, int, int, int, int))
BOTH(long, deep_long, (long, long, long, long, long, long, long, long))

/* the conditions of one type, over every pair of the table */
#define TEST_TYPE(T, N)                                                 \
    static void test_##N(void)                                          \
    {                                                                   \
        for (int i = 0; i < COUNT; ++i) {                               \
            const long x = values[i];                                   \
                                                                        \
            CHECK(not_##N, x, 0, ((T) x));                              \
            CHECK(notnot_##N, x, 0, ((T) x));                           \
            CHECK(truth_##N, x, 0, ((T) x));                            \
            CHECK(false_##N, x, 0, ((T) x));                            \
            CHECK(stale_##N, x, 0, (x));                                \
                                                                        \
            for (int j = 0; j < COUNT; ++j) {                           \
                const long y = values[j];                               \
                                                                        \
                CHECK(lt_##N, x, y, ((T) x, (T) y));                    \
                CHECK(le_##N, x, y, ((T) x, (T) y));                    \
                CHECK(gt_##N, x, y, ((T) x, (T) y));                    \
                CHECK(ge_##N, x, y, ((T) x, (T) y));                    \
                CHECK(eq_##N, x, y, ((T) x, (T) y));                    \
                CHECK(ne_##N, x, y, ((T) x, (T) y));                    \
                CHECK(notlt_##N, x, y, ((T) x, (T) y));                 \
                CHECK(and_##N, x, y, ((T) x, (T) y));                   \
                CHECK(or_##N, x, y, ((T) x, (T) y));                    \
                CHECK(sel_##N, x, y, ((T) x, (T) y));                   \
                CHECK(min_##N, x, y, ((T) x, (T) y));                   \
                CHECK(iflt_##N, x, y, ((T) x, (T) y));                  \
                CHECK(ifle_##N, x, y, ((T) x, (T) y));                  \
                CHECK(ifgt_##N, x, y, ((T) x, (T) y));                  \
                CHECK(ifge_##N, x, y, ((T) x, (T) y));                  \
                CHECK(ifeq_##N, x, y, ((T) x, (T) y));                  \
                CHECK(ifne_##N, x, y, ((T) x, (T) y));                  \
                CHECK(both_##N, x, y, ((T) x, (T) y));                  \
                CHECK(either_##N, x, y, ((T) x, (T) y));                \
                CHECK(neither_##N, x, y, ((T) x, (T) y));               \
                CHECK(stale_not_##N, x, y, (x, y));                     \
                CHECK(stale_and_##N, x, y, (x, y));                     \
                CHECK(mixed_##N, x, y, ((T) x, (T) y));                 \
                CHECK(held_##N, x, y, ((T) x, (T) y));                  \
                CHECK(arms_##N, x, y, ((T) x, (T) y));                  \
            }                                                           \
        }                                                               \
    }

TYPES(TEST_TYPE)

#define TEST_KCMP(T, N, name, op, constant)                             \
    CHECK(k##N##_##name, x, 0, ((T) x));                                \
    CHECK(j##N##_##name, x, 0, ((T) x));
#define TEST_KCMP_ALL(T, N) KCMPS(TEST_KCMP, T, N)

static void
test_constants(void)
{
    for (int i = 0; i < COUNT; ++i) {
        const long x = values[i];

        WIDE(TEST_KCMP_ALL)

        CHECK(constant_conditions, x, 0, ((int) x));

        for (int j = 0; j < COUNT; ++j) {
            const long y = values[j];

            CHECK(arms_mixed, x, y, (0, (int) x, y));
            CHECK(arms_mixed, x, y, (1, (int) x, y));
            CHECK(arms_unsigned, x, y, (0, (unsigned) x, y));
            CHECK(arms_unsigned, x, y, ((int) y, (unsigned) x, y));
        }
    }
}

static void
test_loops(void)
{
    for (int n = -2; n < 60; ++n) {
        CHECK(count, n, 0, (n));
        CHECK(sum, n, 0, (n));
        CHECK(last, n, 0, (n));
        CHECK(pairs, n, 0, (n));
        CHECK(primes, n, 0, (n));

        for (int m = -1; m < 25; ++m) {
            CHECK(nested, n, m, (n, m));
            CHECK(gcd, n, m, (n + 2, m + 1));
        }
    }

    for (unsigned n = 1; n < 300; ++n)
        CHECK(collatz, n, 0, (n));

    for (int i = 0; i < COUNT; ++i)
        CHECK(first_bit, values[i], 0, ((unsigned) values[i]));
}

static void
test_eight(void)
{
    for (int i = 0; i < COUNT; ++i) {
        for (int k = 1; k < COUNT; k += 3) {
            long v[8];

            for (int p = 0; p < 8; ++p)
                v[p] = values[(i + p * k) % COUNT];

            CHECK(deep, i, k,
                  ((int) v[0], (int) v[1], (int) v[2], (int) v[3],
                   (int) v[4], (int) v[5], (int) v[6], (int) v[7]));
            CHECK(deep_long, i, k,
                  (v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7]));
        }
    }
}

int
main(void)
{
#define RUN(T, N) test_##N();
    TYPES(RUN)

    test_constants();
    test_loops();
    test_eight();

    printf("%ld checks, %ld differ\n", total, bad);

    return bad != 0;
}
