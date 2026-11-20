/* The enums probe's main, compiled by clang -O0: calls every function of
   enums.c as rcc compiled it and as clang did (ref_), over a table of
   values and each enumeration's constants, and compares.  Prints each
   difference and a summary; exits 0 when nothing differs, 1 otherwise. */

#include <limits.h>
#include <stdio.h>

#include "enums.h"

static const long values[] = { 0,
                               1,
                               2,
                               3,
                               7,
                               8,
                               'a',
                               127,
                               128,
                               255,
                               256,
                               32767,
                               65535,
                               65536,
                               0x7fffffff,
                               0x80000000L,
                               0x80000001L,
                               0xffffffffL,
                               0x100000000L,
                               0x100000001L,
                               0x123456789abcdef0L,
                               -1,
                               -2,
                               -3,
                               -98,
                               -128,
                               -129,
                               -32768,
                               INT_MIN,
                               -2147483649L,
                               LONG_MAX,
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

#define DECLARE_TYPED(T, N) \
        BOTH(T, id_##N, (T))                \
        BOTH(long, widen_##N, (T))                \
        BOTH(T, cast_##N, (long))                \
        BOTH(long, assign_##N, (long))                \
        BOTH(int, negative_##N, (T))                \
        BOTH(int, below_##N, (T))                \
        BOTH(long, add_##N, (T, int))                \
        BOTH(long, sub_##N, (T, T))                \
        BOTH(int, less_##N, (T, T))                \
        BOTH(long, shift_##N, (T))                \
        BOTH(long, size_##N, (T))                \
        BOTH(long, pointer_##N, (long))                \
        BOTH(long, array_##N, (long))                \
        BOTH(long, global_rw_##N, (long))                \
        BOTH(long, register_##N, (long))                \
        BOTH(int, which_##N, (T))

ENUMS(DECLARE_TYPED)

#define DECLARE_CONSTANT(K) \
        BOTH(long, value_##K, (void))                \
        BOTH(long, size_##K, (void))                \
        BOTH(int, negative_##K, (void))

CONSTANTS(DECLARE_CONSTANT)

BOTH(int, label, (int))
BOTH(long, sizes, (void))
BOTH(long, initial, (int))
BOTH(long, scope, (void))
BOTH(int, listed, (unsigned))
BOTH(long, kinds, (kind_t, kind_t *))

/* the value of each constant, a second table the functions of each type
   are called over */
static const long constants[] = {
#define LISTED(K) K,
        CONSTANTS(LISTED)
};

#define CONSTANT_COUNT ((int) (sizeof constants / sizeof *constants))

/* the functions of one type over the values and the constants, each
   converted to the type by the call; the two-operand ones over the
   constants alone, both ways */
#define TEST_TYPED(T, N)                                                  \
        static void test_##N(void)                                        \
        {                                                                 \
                for (int i = 0; i < COUNT + CONSTANT_COUNT; ++i) {        \
                        const long x =                                    \
                            i < COUNT ? values[i] : constants[i - COUNT]; \
                                                                          \
                        CHECK(id_##N, x, 0, ((T) x));                     \
                        CHECK(widen_##N, x, 0, ((T) x));                  \
                        CHECK(cast_##N, x, 0, (x));                       \
                        CHECK(assign_##N, x, 0, (x));                     \
                        CHECK(negative_##N, x, 0, ((T) x));               \
                        CHECK(below_##N, x, 0, ((T) x));                  \
                        CHECK(add_##N, x, 0, ((T) x, (int) x));           \
                        CHECK(add_##N, x, -1, ((T) x, -1));               \
                        CHECK(shift_##N, x, 0, ((T) x));                  \
                        CHECK(size_##N, x, 0, ((T) x));                   \
                        CHECK(pointer_##N, x, 0, (x));                    \
                        CHECK(array_##N, x, 0, (x));                      \
                        CHECK(global_rw_##N, x, 0, (x));                  \
                        CHECK(register_##N, x, 0, (x));                   \
                        CHECK(which_##N, x, 0, ((T) x));                  \
                }                                                         \
                                                                          \
                for (int i = 0; i < CONSTANT_COUNT; ++i)                  \
                        for (int j = 0; j < CONSTANT_COUNT; ++j) {        \
                                const long x = constants[i];              \
                                const long y = constants[j];              \
                                                                          \
                                CHECK(sub_##N, x, y, ((T) x, (T) y));     \
                                CHECK(less_##N, x, y, ((T) x, (T) y));    \
                        }                                                 \
        }

ENUMS(TEST_TYPED)

#define TEST_CONSTANT(K)            \
        CHECK(value_##K, 0, 0, ()); \
        CHECK(size_##K, 0, 0, ());  \
        CHECK(negative_##K, 0, 0, ());

/* each constant, and the uses of constants that take no value */
static void
test_constants(void)
{
        CONSTANTS(TEST_CONSTANT)

        for (int i = 0; i < COUNT; ++i)
                CHECK(label, values[i], 0, ((int) values[i]));

        for (int x = -10; x < 120; ++x)
                CHECK(label, x, 0, (x));

        CHECK(sizes, 0, 0, ());
        for (int i = 0; i < 11; ++i)
                CHECK(initial, i, 0, (i));

        CHECK(scope, 0, 0, ());
        for (int i = 0; i < COUNT; ++i)
                CHECK(listed, values[i], 0, ((unsigned) values[i]));
}

/* a typedef of an untagged enumeration and a pointer to it: each build
   writes its own object, and the two are compared */
static void
test_kinds(void)
{
        for (int i = 0; i < COUNT; ++i) {
                kind_t a = T0;
                kind_t b = T0;

                const long got = kinds((kind_t) values[i], &a);
                const long want = ref_kinds((kind_t) values[i], &b);

                report("kinds", values[i], 0, got, want);
                report("kinds *p", values[i], 0, a, b);
        }
}

int
main(void)
{
#define CALL_TEST(T, N) test_##N();
        ENUMS(CALL_TEST)

        test_constants();
        test_kinds();

        printf("%ld checks, %ld differ\n", total, bad);

        return bad != 0;
}
