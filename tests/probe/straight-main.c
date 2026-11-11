/* The probe's main, compiled by clang -O0: calls every function of
   straight.c as rcc compiled it and as clang did (ref_), over a table of
   values, and compares.  Prints each difference and a summary; exits 0
   when nothing differs, 1 otherwise. */

#include <limits.h>
#include <stdio.h>

#include "straight.h"

static const long values[] = {
    0, 1, 2, 3, 5, 7, 85, 100, 127, 128, 200, 255, 256, 4095, 4096, 32767,
    32768, 65535, 65536, 0x12345678, 0x7fffffff, 0x80000000L, 0xffffffffL,
    0x100000000L, 0x123456789abcdef0L, -1, -2, -3, -7, -128, -129, -32768,
    -32769, -65536, -2147483647L - 1, -2147483649L, LONG_MAX, LONG_MIN
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
    BOTH(T, add_##N, (T, T)) BOTH(T, sub_##N, (T, T))                   \
    BOTH(T, mul_##N, (T, T)) BOTH(T, div_##N, (T, T))                   \
    BOTH(T, rem_##N, (T, T)) BOTH(T, and_##N, (T, T))                   \
    BOTH(T, or_##N, (T, T)) BOTH(T, xor_##N, (T, T))                    \
    BOTH(T, shl_##N, (T, int)) BOTH(T, shr_##N, (T, int))               \
    BOTH(T, neg_##N, (T)) BOTH(T, not_##N, (T))                         \
    BOTH(T, loc_##N, (T, T)) BOTH(T, val_##N, (T, T))

TYPES(DECLARE_TYPE)

#define DECLARE_CAST(F, FN, T, TN) BOTH(T, cast_##FN##_##TN, (F))
#define DECLARE_CASTS(F, FN) TYPES2(DECLARE_CAST, F, FN)

TYPES(DECLARE_CASTS)

#define DECLARE_KONST(T, N, name, op, constant) BOTH(T, k##N##_##name, (T))
#define DECLARE_KONST_ALL(T, N) KONSTS(DECLARE_KONST, T, N)
#define DECLARE_KONST_64(T, N) KONSTS64(DECLARE_KONST, T, N)

WIDE(DECLARE_KONST_ALL)
DECLARE_KONST_64(long, l)
DECLARE_KONST_64(unsigned long, ul)

#define DECLARE_HELD(name, T, value) BOTH(T, held_##name, (void))

HELD(DECLARE_HELD)

BOTH(long, chain_char, (int))
BOTH(long, chain_int, (long, long))
BOTH(unsigned long, chain_unsigned, (long, long))
BOTH(int, chain_short, (int))
BOTH(long, chain_uchar, (int))
BOTH(long, chain_usum, (unsigned, unsigned))
BOTH(long, chain_sum, (int, int))
BOTH(unsigned long, chain_neg, (int))
BOTH(long, chain_locals, (int))

BOTH(long, mix_sum, (char, short, int, long,
                     unsigned char, unsigned short, unsigned, unsigned long))
BOTH(long, mix_ops, (signed char, short, int, long,
                     unsigned char, unsigned short, unsigned, unsigned long))
BOTH(int, spill, (int, int, int, int, int, int, int, int))
BOTH(long, spill_long, (long, long, long, long, long, long, long, long))
BOTH(long, far, (int, long))
BOTH(int, frame_page, (int))
BOTH(int, frame_two_pages, (int))
BOTH(int, unused, (int, int))
BOTH(int, nothing, (void))

/* the operators, the locals and the assignments of one type.  A
   divisor of 0 or -1 is skipped: the first has no value in C and the
   second none for the least dividend, and what the instruction does
   then is G4's, not this probe's.  A count is below the width of the
   promoted left operand. */
#define TEST_TYPE(T, N)                                                 \
    static void test_##N(void)                                          \
    {                                                                   \
        const int width = (int) (sizeof(+(T) 0) * CHAR_BIT);            \
                                                                        \
        for (int i = 0; i < COUNT; ++i) {                               \
            const long x = values[i];                                   \
                                                                        \
            CHECK(neg_##N, x, 0, ((T) x));                              \
            CHECK(not_##N, x, 0, ((T) x));                              \
                                                                        \
            for (int n = 0; n < width; ++n) {                           \
                CHECK(shl_##N, x, n, ((T) x, n));                       \
                CHECK(shr_##N, x, n, ((T) x, n));                       \
            }                                                           \
                                                                        \
            for (int j = 0; j < COUNT; ++j) {                           \
                const long y = values[j];                               \
                                                                        \
                CHECK(add_##N, x, y, ((T) x, (T) y));                   \
                CHECK(sub_##N, x, y, ((T) x, (T) y));                   \
                CHECK(mul_##N, x, y, ((T) x, (T) y));                   \
                CHECK(and_##N, x, y, ((T) x, (T) y));                   \
                CHECK(or_##N, x, y, ((T) x, (T) y));                    \
                CHECK(xor_##N, x, y, ((T) x, (T) y));                   \
                CHECK(loc_##N, x, y, ((T) x, (T) y));                   \
                CHECK(val_##N, x, y, ((T) x, (T) y));                   \
                                                                        \
                if ((T) y != 0 && (T) y != (T) -1) {                    \
                    CHECK(div_##N, x, y, ((T) x, (T) y));               \
                    CHECK(rem_##N, x, y, ((T) x, (T) y));               \
                }                                                       \
            }                                                           \
        }                                                               \
    }

TYPES(TEST_TYPE)

#define TEST_CAST(F, FN, T, TN) CHECK(cast_##FN##_##TN, x, 0, ((F) x));
#define TEST_CASTS(F, FN) TYPES2(TEST_CAST, F, FN)

#define TEST_KONST(T, N, name, op, constant) CHECK(k##N##_##name, x, 0, ((T) x));
#define TEST_KONST_ALL(T, N) KONSTS(TEST_KONST, T, N)
#define TEST_KONST_64(T, N) KONSTS64(TEST_KONST, T, N)

#define TEST_HELD(name, T, value) CHECK(held_##name, 0, 0, ());

static void
test_one(void)
{
    for (int i = 0; i < COUNT; ++i) {
        const long x = values[i];

        TYPES(TEST_CASTS)

        WIDE(TEST_KONST_ALL)
        TEST_KONST_64(long, l)
        TEST_KONST_64(unsigned long, ul)

        CHECK(chain_char, x, 0, ((int) x));
        CHECK(chain_short, x, 0, ((int) x));
        CHECK(chain_uchar, x, 0, ((int) x));
        CHECK(chain_neg, x, 0, ((int) x));
        CHECK(chain_locals, x, 0, ((int) x));
        CHECK(frame_page, x, 0, ((int) x));
        CHECK(frame_two_pages, x, 0, ((int) x));
    }

    HELD(TEST_HELD)

    CHECK(nothing, 0, 0, ());
}

static void
test_two(void)
{
    for (int i = 0; i < COUNT; ++i) {
        for (int j = 0; j < COUNT; ++j) {
            const long x = values[i];
            const long y = values[j];

            CHECK(chain_int, x, y, (x, y));
            CHECK(chain_unsigned, x, y, (x, y));
            CHECK(chain_usum, x, y, ((unsigned) x, (unsigned) y));
            CHECK(chain_sum, x, y, ((int) x, (int) y));
            CHECK(far, x, y, ((int) x, y));
            CHECK(unused, x, y, ((int) x, (int) y));
        }
    }
}

/* eight arguments, each a different value of the table; the narrow
   ones are passed as the convention has them, the bits above the
   type's width whatever the caller left */
static void
test_eight(void)
{
    for (int i = 0; i < COUNT; ++i) {
        for (int k = 1; k < COUNT; k += 5) {
            long v[8];

            for (int p = 0; p < 8; ++p)
                v[p] = values[(i + p * k) % COUNT];

            CHECK(mix_sum, i, k,
                  ((char) v[0], (short) v[1], (int) v[2], v[3],
                   (unsigned char) v[4], (unsigned short) v[5],
                   (unsigned) v[6], (unsigned long) v[7]));
            CHECK(mix_ops, i, k,
                  ((signed char) v[0], (short) v[1], (int) v[2], v[3],
                   (unsigned char) v[4], (unsigned short) v[5],
                   (unsigned) v[6], (unsigned long) v[7]));
            CHECK(spill, i, k,
                  ((int) v[0], (int) v[1], (int) v[2], (int) v[3],
                   (int) v[4], (int) v[5], (int) v[6], (int) v[7]));
            CHECK(spill_long, i, k,
                  (v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7]));
        }
    }
}

int
main(void)
{
#define RUN(T, N) test_##N();
    TYPES(RUN)

    test_one();
    test_two();
    test_eight();

    printf("%ld checks, %ld differ\n", total, bad);

    return bad != 0;
}
