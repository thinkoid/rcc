/* The calls probe's main, compiled by clang -O0: calls every function
   of calls.c as rcc compiled it and as clang did (ref_), over a table
   of values, and compares.  Prints the first forty differences, or
   every one when PROBE_ALL is set in the environment, and a summary;
   exits 0 when nothing differs, 1 otherwise.

   A function of calls.c whose parameters are narrow is declared here
   with long ones, and is passed whole longs: the bits above the
   parameter's width are whatever the value has there, which the
   convention allows a caller and the callee must not read. */

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include "calls.h"

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
static long shown = 40;

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

/* f and its reference on the same arguments */
#define CHECK(f, x, y, arguments)                                       \
    report(#f, (long) (x), (long) (y), (long) f arguments,              \
           (long) ref_##f arguments)

/* declarations: each function and its reference */
#define BOTH(R, f, parameters) R f parameters; R ref_##f parameters;

#define DECLARE_TYPE(T, N)                                              \
    BOTH(long, par1_##N, (long)) BOTH(long, par8_##N, (P8(long)))       \
    BOTH(long, par9_##N, (P9(long))) BOTH(long, par10_##N, (P10(long))) \
    BOTH(long, step10_##N, (P10(long)))                                 \
    BOTH(T, res_##N, (long)) BOTH(T, inc_##N, (T))                      \
    BOTH(long, tcall9_##N, (T, T)) BOTH(long, tcall10_##N, (T, T))      \
    BOTH(long, chain_##N, (T)) BOTH(long, chain_twice_##N, (T))         \
    BOTH(int, chain_test_##N, (T)) BOTH(long, chain10_##N, (T, T))

TYPES(DECLARE_TYPE)

#define DECLARE_RESULT(F, T, N)                                         \
    BOTH(long, F##_long_##N, (long)) BOTH(int, F##_plus_##N, (long))    \
    BOTH(int, F##_test_##N, (long)) BOTH(int, F##_not_##N, (long))      \
    BOTH(int, F##_less_##N, (long, long)) BOTH(T, F##_return_##N, (long)) \
    BOTH(long, F##_store_##N, (long)) BOTH(long, F##_pass_##N, (long))  \
    BOTH(long, F##_paths_##N, (long, long))

#define DECLARE_RET(T, N) DECLARE_RESULT(ret, T, N)
#define DECLARE_WIDE(T, N) DECLARE_RESULT(wide, T, N)

TYPES(DECLARE_RET)
TYPES(DECLARE_WIDE)

BOTH(long, par12, (P12(long)))
BOTH(long, far_param, (P12(long)))
BOTH(long, call0, (void))
BOTH(long, call1, (long))
BOTH(long, call2, (long, long))
BOTH(long, call3, (long, long, long))
BOTH(long, call4, (long, long, long, long))
BOTH(long, call5, (long, long, long, long, long))
BOTH(long, call6, (long, long, long, long, long, long))
BOTH(long, call7, (long, long, long, long, long, long, long))
BOTH(long, call8, (P8(long)))
BOTH(long, call9, (P8(long)))
BOTH(long, call10, (P8(long)))
BOTH(long, call11, (P8(long)))
BOTH(long, call12, (P8(long)))
BOTH(long, call_constants, (long))
BOTH(long, call_mixed, (long, long))
BOTH(long, variadic, (int, long, char, short))
BOTH(long, effects, (int, int))
BOTH(long, order, (long, long))
BOTH(long, order10, (long))
BOTH(long, held, (long, long, long))
BOTH(long, nest, (long, long, long))
BOTH(long, nest10, (long, long))
BOTH(long, deep_call, (P8(long)))
BOTH(long, deep_argument, (P8(long)))
BOTH(int, condition, (long, long))
BOTH(long, paths, (long, long))
BOTH(int, loop, (int))
BOTH(long, far_call, (long, char, short))
BOTH(int, fact, (int))
BOTH(int, fib, (int))
BOTH(int, even, (int))
BOTH(int, odd, (int))
BOTH(int, ack, (int, int))
BOTH(long, spin, (int, P9(long)))

/* twelve values of the table, a different walk for each i and k */
static void
fill(long v[12], int i, int k)
{
    for (int p = 0; p < 12; ++p)
        v[p] = values[(i + p * k) % COUNT];
}

#define V8 v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7]
#define V9 V8, v[8]
#define V10 V9, v[9]
#define V12 V10, v[10], v[11]

/* the parameters, the arguments and the chain of one type */
#define TEST_TYPE(T, N)                                                 \
    static void test_##N(void)                                          \
    {                                                                   \
        for (int i = 0; i < COUNT; ++i) {                               \
            for (int k = 1; k < COUNT; k += 3) {                        \
                long v[12];                                             \
                                                                        \
                fill(v, i, k);                                          \
                                                                        \
                CHECK(par1_##N, v[0], v[1], (v[0]));                    \
                CHECK(par8_##N, v[0], v[1], (V8));                      \
                CHECK(par9_##N, v[0], v[1], (V9));                      \
                CHECK(par10_##N, v[0], v[1], (V10));                    \
                CHECK(step10_##N, v[0], v[1], (V10));                   \
                CHECK(res_##N, v[0], v[1], (v[0]));                     \
                CHECK(inc_##N, v[0], v[1], ((T) v[0]));                 \
                CHECK(tcall9_##N, v[0], v[1], ((T) v[0], (T) v[1]));    \
                CHECK(tcall10_##N, v[0], v[1], ((T) v[0], (T) v[1]));   \
                CHECK(chain_##N, v[0], v[1], ((T) v[0]));               \
                CHECK(chain_twice_##N, v[0], v[1], ((T) v[0]));         \
                CHECK(chain_test_##N, v[0], v[1], ((T) v[0]));          \
                CHECK(chain10_##N, v[0], v[1], ((T) v[0], (T) v[1]));   \
            }                                                           \
        }                                                               \
    }

TYPES(TEST_TYPE)

/* a result of one type, from an honest function and from a wide one */
#define TEST_RESULT(F, T, N)                                            \
    static void test_##F##_##N(void)                                    \
    {                                                                   \
        for (int i = 0; i < COUNT; ++i) {                               \
            const long x = values[i];                                   \
                                                                        \
            CHECK(F##_long_##N, x, 0, (x));                             \
            CHECK(F##_plus_##N, x, 0, (x));                             \
            CHECK(F##_test_##N, x, 0, (x));                             \
            CHECK(F##_not_##N, x, 0, (x));                              \
            CHECK(F##_return_##N, x, 0, (x));                           \
            CHECK(F##_store_##N, x, 0, (x));                            \
            CHECK(F##_pass_##N, x, 0, (x));                             \
                                                                        \
            for (int j = 0; j < COUNT; ++j) {                           \
                const long y = values[j];                               \
                                                                        \
                CHECK(F##_less_##N, x, y, (x, y));                      \
                CHECK(F##_paths_##N, x, y, (x, y));                     \
            }                                                           \
        }                                                               \
    }

#define TEST_RET(T, N) TEST_RESULT(ret, T, N)
#define TEST_WIDE(T, N) TEST_RESULT(wide, T, N)

TYPES(TEST_RET)
TYPES(TEST_WIDE)

static void
test_calls(void)
{
    for (int i = 0; i < COUNT; ++i) {
        for (int k = 1; k < COUNT; k += 3) {
            long v[12];

            fill(v, i, k);

            CHECK(call0, 0, 0, ());
            CHECK(call1, v[0], v[1], (v[0]));
            CHECK(call2, v[0], v[1], (v[0], v[1]));
            CHECK(call3, v[0], v[1], (v[0], v[1], v[2]));
            CHECK(call4, v[0], v[1], (v[0], v[1], v[2], v[3]));
            CHECK(call5, v[0], v[1], (v[0], v[1], v[2], v[3], v[4]));
            CHECK(call6, v[0], v[1], (v[0], v[1], v[2], v[3], v[4], v[5]));
            CHECK(call7, v[0], v[1],
                  (v[0], v[1], v[2], v[3], v[4], v[5], v[6]));
            CHECK(call8, v[0], v[1], (V8));
            CHECK(call9, v[0], v[1], (V8));
            CHECK(call10, v[0], v[1], (V8));
            CHECK(call11, v[0], v[1], (V8));
            CHECK(call12, v[0], v[1], (V8));
            CHECK(call_constants, v[0], 0, (v[0]));
            CHECK(call_mixed, v[0], v[1], (v[0], v[1]));
            CHECK(par12, v[0], v[1], (V12));
            CHECK(far_param, v[0], v[1], (V12));
            CHECK(variadic, v[0], v[1],
                  ((int) v[0], v[1], (char) v[2], (short) v[3]));
            CHECK(effects, v[0], v[1], ((int) v[0], (int) v[1]));
            CHECK(order, v[0], v[1], (v[0], v[1]));
            CHECK(order10, v[0], 0, (v[0]));
            CHECK(held, v[0], v[1], (v[0], v[1], v[2]));
            CHECK(nest, v[0], v[1], (v[0], v[1], v[2]));
            CHECK(nest10, v[0], v[1], (v[0], v[1]));
            CHECK(deep_call, v[0], v[1], (V8));
            CHECK(deep_argument, v[0], v[1], (V8));
            CHECK(condition, v[0], v[1], (v[0], v[1]));
            CHECK(paths, v[0], v[1], (v[0], v[1]));
            CHECK(far_call, v[0], v[1], (v[0], (char) v[1], (short) v[2]));
            CHECK(spin, i % 7, v[0], (i % 7, V9));
        }
    }
}

static void
test_recursion(void)
{
    for (int n = -2; n < 40; ++n) {
        CHECK(loop, n, 0, (n));
        CHECK(spin, n + 2, 0, (n + 2, 1, 2, 3, 4, 5, 6, 7, 8, 9));
    }

    for (int n = 0; n < 13; ++n)
        CHECK(fact, n, 0, (n));

    for (int n = 0; n < 21; ++n)
        CHECK(fib, n, 0, (n));

    for (int n = 0; n < 30; ++n) {
        CHECK(even, n, 0, (n));
        CHECK(odd, n, 0, (n));
    }

    for (int m = 0; m < 3; ++m)
        for (int n = 0; n < 6; ++n)
            CHECK(ack, m, n, (m, n));
}

int
main(void)
{
    if (getenv("PROBE_ALL"))
        shown = LONG_MAX;

#define RUN(T, N) test_##N();
    TYPES(RUN)

#define RUN_RET(T, N) test_ret_##N();
    TYPES(RUN_RET)

#define RUN_WIDE(T, N) test_wide_##N();
    TYPES(RUN_WIDE)

    test_calls();
    test_recursion();

    printf("%ld checks, %ld differ\n", total, bad);

    return bad != 0;
}
