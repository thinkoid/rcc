/* The switches probe's main, compiled by clang -O0: calls every
   function of switches.c as rcc compiled it and as clang did (ref_),
   over a table of values, and compares.  The table holds every case
   value, each one's neighbours and the limits of each type: the values
   below, each less one, as it is and plus one, with the arithmetic of
   unsigned long, which wraps.  Prints each difference and a summary;
   exits 0 when nothing differs, 1 otherwise. */

#include <limits.h>
#include <stdio.h>

#include "switches.h"

static const long values[] = { 0,
                               1,
                               2,
                               3,
                               4,
                               5,
                               6,
                               7,
                               8,
                               9,
                               10,
                               11,
                               12,
                               13,
                               17,
                               20,
                               42,
                               50,
                               77,
                               'a',
                               'b',
                               'c',
                               'd',
                               'e',
                               100,
                               101,
                               102,
                               104,
                               107,
                               108,
                               123,
                               127,
                               200,
                               300,
                               700,
                               4000,
                               4096,
                               4567,
                               5000,
                               9000,
                               30000,
                               60000,
                               65535,
                               65537,
                               70000,
                               88888,
                               700000,
                               1000000,
                               123456789,
                               -1,
                               -2,
                               -3,
                               -5,
                               -7,
                               -9,
                               -20,
                               -80,
                               -100,
                               -32768,
                               INT_MIN,
                               INT_MIN + 1,
                               INT_MIN + 3,
                               INT_MIN + 5,
                               INT_MAX,
                               INT_MAX - 1,
                               INT_MAX - 2,
                               INT_MAX - 3,
                               UINT_MAX,
                               UINT_MAX - 1,
                               UINT_MAX - 2,
                               UINT_MAX - 3,
                               UINT_MAX - 7,
                               UINT_MAX - 99,
                               LONG_MIN,
                               LONG_MIN + 1,
                               LONG_MIN + 3,
                               LONG_MIN + 5,
                               LONG_MAX,
                               LONG_MAX - 1,
                               LONG_MAX - 2,
                               LONG_MAX - 3 };

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
                printf("%s(%ld, %ld): rcc %ld, clang %ld\n", name, x, y, got,
                       want);
}

/* f and its reference on the same arguments */
#define CHECK(f, x, y, arguments)                              \
        report(#f, (long) (x), (long) (y), (long) f arguments, \
               (long) ref_##f arguments)

/* declarations: each function and its reference */
#define BOTH(R, f, parameters) \
        R f parameters;        \
        R ref_##f parameters;

#define DECLARE_TYPE(T, N, LO, HI) \
        BOTH(int, direct_zero_##N, (T))                       \
        BOTH(int, direct_up_##N, (T))                       \
        BOTH(int, direct_down_##N, (T))                       \
        BOTH(int, direct_top_##N, (T))                       \
        BOTH(int, direct_bottom_##N, (T))                       \
        BOTH(int, edge_direct_##N, (T))                       \
        BOTH(int, edge_simple_##N, (T))                       \
        BOTH(int, simple_##N, (T))                       \
        BOTH(int, simple_limits_##N, (T))                       \
        BOTH(int, single_##N, (T))                       \
        BOTH(int, nine_##N, (T))                       \
        BOTH(int, none_##N, (T))                       \
        BOTH(int, only_default_##N, (T))                       \
        BOTH(int, hash_##N, (T))                       \
        BOTH(int, fall_##N, (T))                       \
        BOTH(int, register_##N, (T))

LIMITS(DECLARE_TYPE)

#define DECLARE_NARROW(T, N) \
        BOTH(int, narrow_##N, (T))                 \
        BOTH(int, letters_##N, (T))

NARROW(DECLARE_NARROW)

BOTH(int, nested, (int, int))
BOTH(long, in_loop, (int))
BOTH(int, loop_in_switch, (int))
BOTH(int, nested_block, (int))
BOTH(int, labels, (int))
BOTH(int, duff, (int))
BOTH(int, once, (int))

/* the switches of one type over x */
#define TEST_TYPE(T, N, LO, HI)                  \
        CHECK(direct_zero_##N, x, 0, ((T) x));   \
        CHECK(direct_up_##N, x, 0, ((T) x));     \
        CHECK(direct_down_##N, x, 0, ((T) x));   \
        CHECK(direct_top_##N, x, 0, ((T) x));    \
        CHECK(direct_bottom_##N, x, 0, ((T) x)); \
        CHECK(edge_direct_##N, x, 0, ((T) x));   \
        CHECK(edge_simple_##N, x, 0, ((T) x));   \
        CHECK(simple_##N, x, 0, ((T) x));        \
        CHECK(simple_limits_##N, x, 0, ((T) x)); \
        CHECK(single_##N, x, 0, ((T) x));        \
        CHECK(nine_##N, x, 0, ((T) x));          \
        CHECK(none_##N, x, 0, ((T) x));          \
        CHECK(only_default_##N, x, 0, ((T) x));  \
        CHECK(hash_##N, x, 0, ((T) x));          \
        CHECK(fall_##N, x, 0, ((T) x));          \
        CHECK(register_##N, x, 0, ((T) x));

#define TEST_NARROW(T, N)                 \
        CHECK(narrow_##N, x, 0, ((T) x)); \
        CHECK(letters_##N, x, 0, ((T) x));

/* every function over x */
static void
test(long x)
{
        LIMITS(TEST_TYPE)
        NARROW(TEST_NARROW)

        CHECK(loop_in_switch, x, 0, ((int) x));
        CHECK(nested_block, x, 0, ((int) x));
        CHECK(labels, x, 0, ((int) x));
        CHECK(once, x, 0, ((int) x));
}

static void
test_small(void)
{
        for (int n = -5; n < 60; ++n) {
                CHECK(in_loop, n, 0, (n));
                CHECK(loop_in_switch, n, 0, (n));
                CHECK(duff, n, 0, (n));

                for (int m = -2; m < 6; ++m)
                        CHECK(nested, n, m, (n, m));
        }
}

int
main(void)
{
        for (int i = 0; i < COUNT; ++i)
                for (int d = -1; d <= 1; ++d)
                        test((long) ((unsigned long) values[i] + d));

        test_small();

        printf("%ld checks, %ld differ\n", total, bad);

        return bad != 0;
}
