/* The jumps probe's main, compiled by clang -O0: calls every function
   of jumps.c as rcc compiled it and as clang did (ref_), over a table
   of values, and compares.  Prints each difference and a summary;
   exits 0 when nothing differs, 1 otherwise. */

#include <limits.h>
#include <stdio.h>

#include "jumps.h"

static const long values[] = { 0,
                               1,
                               2,
                               3,
                               5,
                               7,
                               85,
                               100,
                               127,
                               128,
                               200,
                               255,
                               256,
                               4095,
                               4096,
                               4097,
                               5000,
                               32767,
                               32768,
                               65535,
                               65536,
                               0x12345,
                               0xfff000,
                               0x12345678,
                               0x7fffffff,
                               0x80000000L,
                               0xffffffffL,
                               0x100000000L,
                               0x123456789abcdef0L,
                               -1,
                               -2,
                               -3,
                               -7,
                               -128,
                               -129,
                               -4096,
                               -5000,
                               -32768,
                               -32769,
                               -65536,
                               -2147483647L - 1,
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

#define DECLARE_TYPE(T, N) \
        BOTH(int, dlt_##N, (T, T))               \
        BOTH(int, dle_##N, (T, T))               \
        BOTH(int, dgt_##N, (T, T))               \
        BOTH(int, dge_##N, (T, T))               \
        BOTH(int, deq_##N, (T, T))               \
        BOTH(int, dne_##N, (T, T))               \
        BOTH(int, dtruth_##N, (T))               \
        BOTH(int, dnot_##N, (T))               \
        BOTH(int, dnotlt_##N, (T, T))               \
        BOTH(int, dand_##N, (T, T))               \
        BOTH(int, dor_##N, (T, T))               \
        BOTH(int, dneither_##N, (T, T))               \
        BOTH(int, dstale_##N, (long))               \
        BOTH(int, dmixed_##N, (T, T))

TYPES(DECLARE_TYPE)

#define DECLARE_KCMP(T, N, name, op, constant) BOTH(int, d##N##_##name, (T))
#define DECLARE_KCMP_ALL(T, N)                 KCMPS(DECLARE_KCMP, T, N)

WIDE(DECLARE_KCMP_ALL)

BOTH(int, count_up, (int))
BOTH(int, digits, (unsigned))
BOTH(int, commas, (int))
BOTH(int, constants, (int))
BOTH(int, empty_body, (int))
BOTH(int, while_jumps, (int))
BOTH(int, do_jumps, (int))
BOTH(int, for_jumps, (int))
BOTH(int, for_ever, (int))
BOTH(long, nested_jumps, (int, int))
BOTH(int, nested_do, (int))
BOTH(int, skip, (int))
BOTH(int, goto_loop, (int))
BOTH(int, find, (int, int))
BOTH(int, into_block, (int))
BOTH(int, into_loop, (int))
BOTH(int, registers, (int))
BOTH(int, register_into, (int))
BOTH(int, later_block, (int))
BOTH(int, later_for, (int))
BOTH(int, later_deep, (int))
BOTH(int, later_loop, (int))
BOTH(int, later_case, (int))
BOTH(int, label_declaration, (int))
BOTH(int, label_end, (int))
BOTH(void, clamp, (int *))
BOTH(int, names, (int))

#define DECLARE_IF_JUMP(row, loop, condition) \
        BOTH(void, if_##row##_##loop##_##condition, (int, int, int *))

IF_JUMPS(DECLARE_IF_JUMP)

/* the conditions of one type, over every pair of the table */
#define TEST_TYPE(T, N)                                                    \
        static void test_##N(void)                                         \
        {                                                                  \
                for (int i = 0; i < COUNT; ++i) {                          \
                        const long x = values[i];                          \
                                                                           \
                        CHECK(dtruth_##N, x, 0, ((T) x));                  \
                        CHECK(dnot_##N, x, 0, ((T) x));                    \
                        CHECK(dstale_##N, x, 0, (x));                      \
                                                                           \
                        for (int j = 0; j < COUNT; ++j) {                  \
                                const long y = values[j];                  \
                                                                           \
                                CHECK(dlt_##N, x, y, ((T) x, (T) y));      \
                                CHECK(dle_##N, x, y, ((T) x, (T) y));      \
                                CHECK(dgt_##N, x, y, ((T) x, (T) y));      \
                                CHECK(dge_##N, x, y, ((T) x, (T) y));      \
                                CHECK(deq_##N, x, y, ((T) x, (T) y));      \
                                CHECK(dne_##N, x, y, ((T) x, (T) y));      \
                                CHECK(dnotlt_##N, x, y, ((T) x, (T) y));   \
                                CHECK(dand_##N, x, y, ((T) x, (T) y));     \
                                CHECK(dor_##N, x, y, ((T) x, (T) y));      \
                                CHECK(dneither_##N, x, y, ((T) x, (T) y)); \
                                CHECK(dmixed_##N, x, y, ((T) x, (T) y));   \
                        }                                                  \
                }                                                          \
        }

TYPES(TEST_TYPE)

#define TEST_KCMP(T, N, name, op, constant) CHECK(d##N##_##name, x, 0, ((T) x));
#define TEST_KCMP_ALL(T, N)                 KCMPS(TEST_KCMP, T, N)

static void
test_constants(void)
{
        for (int i = 0; i < COUNT; ++i) {
                const long x = values[i];

                WIDE(TEST_KCMP_ALL)

                CHECK(digits, x, 0, ((unsigned) x));
                CHECK(constants, x, 0, ((int) x));
                CHECK(skip, x, 0, ((int) x));
                CHECK(into_block, x, 0, ((int) x));
                CHECK(names, x, 0, ((int) x));
        }
}

/* clamp() writes through its pointer; the result is what it left */
static int
clamped(void (*f)(int *), int x)
{
        f(&x);

        return x;
}

static void
test_loops(void)
{
        for (int n = -5; n < 60; ++n) {
                CHECK(count_up, n, 0, (n));
                CHECK(commas, n, 0, (n));
                CHECK(empty_body, n, 0, (n));
                CHECK(while_jumps, n, 0, (n));
                CHECK(do_jumps, n, 0, (n));
                CHECK(for_jumps, n, 0, (n));
                CHECK(for_ever, n, 0, (n));
                CHECK(nested_do, n, 0, (n));
                CHECK(skip, n, 0, (n));
                CHECK(goto_loop, n, 0, (n));
                CHECK(into_block, n, 0, (n));
                CHECK(into_loop, n, 0, (n));
                CHECK(registers, n, 0, (n));
                CHECK(register_into, n, 0, (n));
                CHECK(later_block, n, 0, (n));
                CHECK(later_for, n, 0, (n));
                CHECK(later_deep, n, 0, (n));
                CHECK(later_loop, n, 0, (n));
                CHECK(later_case, n, 0, (n));
                CHECK(label_declaration, n, 0, (n));
                CHECK(label_end, n, 0, (n));
                report("clamp", n, 0, clamped(clamp, n), clamped(ref_clamp, n));

                for (int m = -1; m < 25; ++m) {
                        CHECK(nested_jumps, n, m, (n, m));
                        CHECK(find, n, m, (n, m * 3));
                }
        }
}

/* an if_ function's result: what it left in *r */
static int
left(void (*f)(int, int, int *), int a, int b)
{
        int r;

        f(a, b, &r);

        return r;
}

#define TEST_IF_JUMP(row, loop, condition)                  \
        report("if_" #row "_" #loop "_" #condition, a, b,   \
               left(if_##row##_##loop##_##condition, a, b), \
               left(ref_if_##row##_##loop##_##condition, a, b));

/* every row of an if whose body is one jump, over a and b from -3 to
   14, which take each condition both ways at some pass of the twelve */
static void
test_if_jumps(void)
{
        for (int a = -3; a < 15; ++a)
                for (int b = -3; b < 15; ++b) {
                        IF_JUMPS(TEST_IF_JUMP)
                }
}

int
main(void)
{
#define RUN(T, N) test_##N();
        TYPES(RUN)

        test_constants();
        test_loops();
        test_if_jumps();

        printf("%ld checks, %ld differ\n", total, bad);

        return bad != 0;
}
