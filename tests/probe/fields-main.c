/* The fields probe's main, compiled by clang -O0: calls every function
   of fields.c as rcc compiled it and as clang did (ref_), each on the
   bytes of its record filled by a set of patterns, and compares.
   Prints each difference and a summary; exits 0 when nothing differs,
   1 otherwise. */

#include <stdio.h>

#include "fields.h"

static long total;
static long bad;

static void
report(const char *name, int pattern, long got, long want)
{
        ++total;

        if (got == want)
                return;

        if (++bad <= 40)
                printf("%s, pattern %d: rcc %ld, clang %ld\n", name, pattern,
                       got, want);
}

/* the bytes a record is read from */
static unsigned char bytes[64] __attribute__((aligned(16)));

#define PATTERNS 6

/* fills the size bytes of a record by pattern k: all zeros, all ones,
   a mix and its complement, the top bit of each byte, and the low bit
   of each */
static void
fill(int k, long size)
{
        for (long i = 0; i < size; ++i) {
                const unsigned char mix = (unsigned char) (0x5a ^ (i * 37));

                bytes[i] = k == 0 ? 0 :
                           k == 1 ? 0xff :
                           k == 2 ? mix :
                           k == 3 ? (unsigned char) ~mix :
                           k == 4 ? 0x80 :
                                    0x01;
        }
}

/* each function of fields.c, as rcc compiled it and as clang did */
#define DECLARE(N, MEMBERS)                                 \
        long get_##N(struct N *), ref_get_##N(struct N *);  \
        int test_##N(struct N *), ref_test_##N(struct N *); \
        int zero_##N(struct N *), ref_zero_##N(struct N *); \
        int neg_##N(struct N *), ref_neg_##N(struct N *);

#define CASE_DECLARE(N, T, W, PAD) SHAPES(DECLARE, N, T, W, PAD)

CASES(CASE_DECLARE)

/* the reads of record N on each pattern */
#define CHECK(N, MEMBERS)                                            \
        for (int k = 0; k < PATTERNS; ++k) {                         \
                struct N *p = (struct N *) bytes;                    \
                                                                     \
                fill(k, sizeof(struct N));                           \
                report("get_" #N, k, get_##N(p), ref_get_##N(p));    \
                report("test_" #N, k, test_##N(p), ref_test_##N(p)); \
                report("zero_" #N, k, zero_##N(p), ref_zero_##N(p)); \
                report("neg_" #N, k, neg_##N(p), ref_neg_##N(p));    \
        }

#define CASE_CHECK(N, T, W, PAD) SHAPES(CHECK, N, T, W, PAD)

int
main(void)
{
        CASES(CASE_CHECK)

        printf("%ld checks, %ld differ\n", total, bad);

        return bad != 0;
}
