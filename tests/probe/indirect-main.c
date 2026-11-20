/* The indirect probe's main, compiled by clang -O0: calls every
   function of indirect.c as rcc compiled it and as clang did (ref_),
   over a table of values, and compares.  It calls through the pointers
   they return, to rcc's functions and to clang's, and passes them
   pointers to functions of its own.  Prints the first forty
   differences, or every one when PROBE_ALL is set in the environment,
   and a summary; exits 0 when nothing differs, 1 otherwise.

   A function of indirect.c whose parameters are narrow is declared here
   with long ones where the calls probe does, and is passed whole longs:
   the bits above the parameter's width are whatever the value has
   there, which the convention allows a caller and the callee must not
   read. */

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include "indirect.h"

static const long values[] = { 0,
                               1,
                               2,
                               3,
                               5,
                               7,
                               85,
                               127,
                               128,
                               255,
                               256,
                               4096,
                               5000,
                               32767,
                               32768,
                               65535,
                               65536,
                               0x12345678,
                               0x7fffffff,
                               0x80000000L,
                               0xffffffffL,
                               0x123456789abcdef0L,
                               -1,
                               -2,
                               -128,
                               -129,
                               -32768,
                               -32769,
                               -2147483648L,
                               LONG_MAX,
                               LONG_MIN };

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
                printf("%s(%ld, %ld, ...): rcc %ld, clang %ld\n", name, x, y,
                       got, want);
}

/* f and its reference on the same arguments */
#define CHECK(f, x, y, arguments)                              \
        report(#f, (long) (x), (long) (y), (long) f arguments, \
               (long) ref_##f arguments)

/* declarations: each function and its reference */
#define BOTH(R, f, parameters) \
        R f parameters;        \
        R ref_##f parameters;

/* indirect-lib.c, linked as it is and under the prefix */
BOTH(long, mix1, (long))
BOTH(long, mix2, (long, long))

BOTH(long, twice, (long))
BOTH(fn1_t, defined_address, (void))
BOTH(fn1_t, declared_address, (void))
BOTH(int, declared_same, (void))
BOTH(int, defined_same, (void))
BOTH(int, null_tests, (fn1_t))
BOTH(int, address_forms, (int))
BOTH(long, through_local, (long))
BOTH(long, through_parameter, (fn1_t, long))
BOTH(long, through_register, (long, long))
BOTH(long, through_array, (int, long))
BOTH(fn1_t, pick, (int))
BOTH(long, through_call, (int, long))
BOTH(long, through_conditional, (int, long, long))
BOTH(long, through_cast, (long, long))
BOTH(long, through_name, (long))
BOTH(long, ifact, (long))
BOTH(fn1_t, ifact_address, (void))
BOTH(long, icall0, (void))
BOTH(long, icall1, (long))
BOTH(long, icall2, (long, long))
BOTH(long, icall3, (long, long, long))
BOTH(long, icall4, (long, long, long, long))
BOTH(long, icall5, (long, long, long, long, long))
BOTH(long, icall6, (long, long, long, long, long, long))
BOTH(long, icall7, (long, long, long, long, long, long, long))
BOTH(long, icall8, (P8(long)))
BOTH(long, icall9, (P8(long)))
BOTH(long, icall10, (P8(long)))
BOTH(long, icall11, (P8(long)))
BOTH(long, icall12, (P8(long)))
BOTH(long, icall_mixed, (long, long))
BOTH(long, ivariadic, (int, long, char, short))
BOTH(long, ieffects, (int, int))
BOTH(long, callee_first, (long))
BOTH(long, callee_order, (long, long))
BOTH(long, iheld, (long, long, long))
BOTH(long, inest, (long, long, long))
BOTH(long, ideep, (P8(long)))
BOTH(int, icondition, (long, long))
BOTH(long, ipaths, (long, long))
BOTH(long, far_icall, (long, char, short))

#define DECLARE_TYPE(T, N) \
        BOTH(T, inc_##N, (T))               \
        BOTH(inc_##N##_t, pick_inc_##N, (void))               \
        BOTH(long, ichain_##N, (T))               \
        BOTH(long, iret_##N, (long))               \
        BOTH(long, itmix10_##N, (T, T))               \
        BOTH(long, ipar10_##N, (T, T))               \
        BOTH(long, iapply_##N, (inc_##N##_t, T))

TYPES(DECLARE_TYPE)

/* The main's own functions, which rcc's code calls through the pointers
   it is passed: the second pairing from the main's side. */
static long
thrice(long a)
{
        return a * 3 + 1;
}

#define OWN(T, N)                 \
        static T own_inc_##N(T a) \
        {                         \
                return a + 7;     \
        }

TYPES(OWN)

/* A function's address: rcc's of a function rcc's file defines is the
   main's of the same function, and rcc's of one it only declares is the
   main's and the library's; as clang's are, each of its own build. */
static void
test_addresses(void)
{
        report("defined_address", 0, 0, defined_address() == twice,
               ref_defined_address() == ref_twice);
        report("declared_address", 0, 0, declared_address() == mix1,
               ref_declared_address() == ref_mix1);
        report("ifact_address", 0, 0, ifact_address() == ifact,
               ref_ifact_address() == ref_ifact);
        report("pick", 0, 0, pick(0) == mix1, ref_pick(0) == ref_mix1);
        report("pick", 1, 0, pick(1) == twice, ref_pick(1) == ref_twice);

        CHECK(declared_same, 0, 0, ());
        CHECK(defined_same, 0, 0, ());
        CHECK(null_tests, 0, 0, (0));
        CHECK(null_tests, 1, 0, (thrice));
        CHECK(address_forms, 0, 0, (0));
        CHECK(address_forms, 1, 0, (1));
}

/* the calls through pointers of each type: from rcc's code to rcc's and
   to clang's, and from the main to rcc's through pick_inc */
#define TEST_TYPE(T, N)                                                     \
        static void test_##N(void)                                          \
        {                                                                   \
                for (int i = 0; i < COUNT; ++i) {                           \
                        const long x = values[i];                           \
                                                                            \
                        report("pick_inc_" #N, x, 0, pick_inc_##N()((T) x), \
                               ref_pick_inc_##N()((T) x));                  \
                        CHECK(ichain_##N, x, 0, ((T) x));                   \
                        CHECK(iret_##N, x, 0, (x));                         \
                        CHECK(iapply_##N, x, 0, (own_inc_##N, (T) x));      \
                                                                            \
                        for (int j = 0; j < COUNT; j += 3) {                \
                                const long y = values[j];                   \
                                                                            \
                                CHECK(itmix10_##N, x, y, ((T) x, (T) y));   \
                                CHECK(ipar10_##N, x, y, ((T) x, (T) y));    \
                        }                                                   \
                }                                                           \
        }

TYPES(TEST_TYPE)

/* twelve values of the table, a different walk for each i and k */
static void
fill(long v[12], int i, int k)
{
        for (int p = 0; p < 12; ++p)
                v[p] = values[(i + p * k) % COUNT];
}

#define V8 v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7]

static void
test_calls(void)
{
        for (int i = 0; i < COUNT; ++i) {
                for (int k = 1; k < COUNT; k += 3) {
                        long v[12];

                        fill(v, i, k);

                        report("pick(k)", k % 2, v[0], pick(k % 2)(v[0]),
                               ref_pick(k % 2)(v[0]));
                        CHECK(through_local, v[0], 0, (v[0]));
                        CHECK(through_parameter, v[0], 0, (thrice, v[0]));
                        CHECK(through_parameter, v[0], 1, (twice, v[0]));
                        CHECK(through_register, v[0], v[1], (v[0], v[1]));
                        CHECK(through_array, i % 3, v[0], (i % 3, v[0]));
                        CHECK(through_call, k % 2, v[0], (k % 2, v[0]));
                        CHECK(through_conditional, k % 2, v[0],
                              (k % 2, v[0], v[1]));
                        CHECK(through_cast, v[0], v[1], (v[0], v[1]));
                        CHECK(through_name, v[0], 0, (v[0]));
                        CHECK(icall0, 0, 0, ());
                        CHECK(icall1, v[0], v[1], (v[0]));
                        CHECK(icall2, v[0], v[1], (v[0], v[1]));
                        CHECK(icall3, v[0], v[1], (v[0], v[1], v[2]));
                        CHECK(icall4, v[0], v[1], (v[0], v[1], v[2], v[3]));
                        CHECK(icall5, v[0], v[1],
                              (v[0], v[1], v[2], v[3], v[4]));
                        CHECK(icall6, v[0], v[1],
                              (v[0], v[1], v[2], v[3], v[4], v[5]));
                        CHECK(icall7, v[0], v[1],
                              (v[0], v[1], v[2], v[3], v[4], v[5], v[6]));
                        CHECK(icall8, v[0], v[1], (V8));
                        CHECK(icall9, v[0], v[1], (V8));
                        CHECK(icall10, v[0], v[1], (V8));
                        CHECK(icall11, v[0], v[1], (V8));
                        CHECK(icall12, v[0], v[1], (V8));
                        CHECK(icall_mixed, v[0], v[1], (v[0], v[1]));
                        CHECK(ivariadic, v[0], v[1],
                              ((int) v[0], v[1], (char) v[2], (short) v[3]));
                        CHECK(ieffects, v[0], v[1], ((int) v[0], (int) v[1]));
                        CHECK(callee_first, v[0], 0, (v[0]));
                        CHECK(callee_order, v[0], v[1], (v[0], v[1]));
                        CHECK(iheld, v[0], v[1], (v[0], v[1], v[2]));
                        CHECK(inest, v[0], v[1], (v[0], v[1], v[2]));
                        CHECK(ideep, v[0], v[1], (V8));
                        CHECK(icondition, v[0], v[1], (v[0], v[1]));
                        CHECK(ipaths, v[0], v[1], (v[0], v[1]));
                        CHECK(far_icall, v[0], v[1],
                              (v[0], (char) v[1], (short) v[2]));
                }
        }

        for (int n = -2; n < 21; ++n)
                CHECK(ifact, n, 0, (n));
}

int
main(void)
{
        if (getenv("PROBE_ALL"))
                shown = LONG_MAX;

        test_addresses();

#define RUN(T, N) test_##N();
        TYPES(RUN)

        test_calls();

        printf("%ld checks, %ld differ\n", total, bad);

        return bad != 0;
}
