/* The bools probe's main, compiled by clang -O0: calls every function
   of bools.c as rcc compiled it and as clang did (ref_), over a table of
   values and the two values of a _Bool, and compares.  Prints each
   difference and a summary; exits 0 when nothing differs, 1
   otherwise. */

#include <limits.h>
#include <stdio.h>

#include "bools.h"

static const long values[] = { 0,
                               1,
                               2,
                               3,
                               7,
                               8,
                               255,
                               256,
                               0x7fffffff,
                               0x80000000L,
                               0xffffffffL,
                               0x100000000L,
                               0x100000001L,
                               -1,
                               -2,
                               -256,
                               INT_MIN,
                               LONG_MIN };

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

/* clang-format off */
#define DECLARE_CONVERSION(T, N)        \
        BOTH(_Bool, to_bool_##N, (T))   \
        BOTH(_Bool, cast_bool_##N, (T)) \
        BOTH(int, stored_##N, (T))
#define DECLARE_COMPOUND(OP, N)             \
        BOTH(int, stores_##N, (_Bool, int)) \
        BOTH(int, yields_##N, (_Bool, int))
/* clang-format on */
#define DECLARE_STEP(N, S) BOTH(int, N, (_Bool))

CONVERSIONS(DECLARE_CONVERSION)
COMPOUNDS(DECLARE_COMPOUND)
DIVIDING(DECLARE_COMPOUND)
SHIFTS(DECLARE_COMPOUND)
STEPS(DECLARE_STEP)
BOTH(_Bool, from_pointer, (char *))
BOTH(_Bool, less, (int, int))
BOTH(_Bool, not, (int))
BOTH(_Bool, both, (int, int))
BOTH(_Bool, either, (int, int))
BOTH(int, arithmetic, (_Bool, _Bool))
BOTH(int, condition, (_Bool))
BOTH(int, negated, (_Bool))
BOTH(_Bool, copied, (_Bool))
BOTH(int, members, (int))
BOTH(int, elements, (int))
BOTH(int, global, (int))
BOTH(int, calls, (int))
BOTH(int, selected, (_Bool))

#define CHECK_CONVERSION(T, N)                   \
        for (int k = 0; k < COUNT; ++k) {        \
                const T x = (T) values[k];       \
                CHECK(to_bool_##N, x, 0, (x));   \
                CHECK(cast_bool_##N, x, 0, (x)); \
                CHECK(stored_##N, x, 0, (x));    \
        }
#define CHECK_COMPOUND(OP, N)                            \
        for (int a = 0; a < 2; ++a)                      \
                for (int k = 0; k < COUNT; ++k) {        \
                        const int x = (int) values[k];   \
                        CHECK(stores_##N, a, x, (a, x)); \
                        CHECK(yields_##N, a, x, (a, x)); \
                }
#define CHECK_DIVIDING(OP, N)                            \
        for (int a = 0; a < 2; ++a)                      \
                for (int k = 0; k < COUNT; ++k) {        \
                        const int x = (int) values[k];   \
                        if (x == 0)                      \
                                continue;                \
                        CHECK(stores_##N, a, x, (a, x)); \
                        CHECK(yields_##N, a, x, (a, x)); \
                }
#define CHECK_STEP(N, S)            \
        for (int a = 0; a < 2; ++a) \
                CHECK(N, a, 0, (a));

/* the _Bool functions over both values of a _Bool and a pair of each */
static void
check_bools(void)
{
        for (int a = 0; a < 2; ++a) {
                CHECK(condition, a, 0, (a));
                CHECK(negated, a, 0, (a));
                CHECK(copied, a, 0, (a));
                CHECK(selected, a, 0, (a));

                for (int b = 0; b < 2; ++b)
                        CHECK(arithmetic, a, b, (a, b));
        }
}

/* the functions of integers over the table */
static void
check_integers(void)
{
        for (int k = 0; k < COUNT; ++k) {
                const int x = (int) values[k];

                CHECK(from_pointer, values[k], 0, ((char *) values[k]));
                CHECK(not, x, 0, (x));
                CHECK(members, x, 0, (x));
                CHECK(elements, x, 0, (x));
                CHECK(global, x, 0, (x));
                CHECK(calls, x, 0, (x));

                for (int j = 0; j < COUNT; ++j) {
                        const int y = (int) values[j];

                        CHECK(less, x, y, (x, y));
                        CHECK(both, x, y, (x, y));
                        CHECK(either, x, y, (x, y));
                }
        }
}

int
main(void)
{
        CONVERSIONS(CHECK_CONVERSION)
        COMPOUNDS(CHECK_COMPOUND)
        DIVIDING(CHECK_DIVIDING)
        SHIFTS(CHECK_COMPOUND)
        STEPS(CHECK_STEP)
        check_bools();
        check_integers();

        printf("%ld checks, %ld differ\n", total, bad);

        return bad != 0;
}
