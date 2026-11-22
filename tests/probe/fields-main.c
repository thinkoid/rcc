/* The fields probe's main, compiled by clang -O0: calls every function
   of fields.c as rcc compiled it and as clang did (ref_), each on the
   bytes of its record filled by a set of patterns, the stores over a
   table of values, and compares what each returns and, after a store,
   every byte of the record.  Then the two order cases.  Prints each
   difference and a summary; exits 0 when nothing differs, 1
   otherwise. */

#include <stdio.h>
#include <string.h>

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

/* the bytes of a record after a store, rcc's and clang's, compared */
static void
report_bytes(const char *name,
             int pattern,
             long value,
             const unsigned char *got,
             const unsigned char *want,
             long size)
{
        ++total;

        if (memcmp(got, want, size) == 0)
                return;

        if (++bad > 40)
                return;

        printf("%s, pattern %d, value %ld: rcc", name, pattern, value);
        for (long i = 0; i < size; ++i)
                printf(" %02x", got[i]);
        printf(", clang");
        for (long i = 0; i < size; ++i)
                printf(" %02x", want[i]);
        printf("\n");
}

/* the bytes a record is read from and stored into, and a copy */
static unsigned char bytes[64] __attribute__((aligned(16)));
static unsigned char saved[64];

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

/* the values stored */
static const long values[] = { 0,
                               1,
                               -1,
                               2,
                               -2,
                               5,
                               0x7f,
                               0x80,
                               0x7fff,
                               0x8000,
                               0x7fffffff,
                               (long) 0x80000000UL,
                               0x123456789abcdefL,
                               (long) 0x8000000000000000UL };

#define VALUES ((int) (sizeof values / sizeof *values))

/* each function of fields.c, as rcc compiled it and as clang did */
#define DECLARE(N, MEMBERS)                                                \
        long get_##N(struct N *), ref_get_##N(struct N *);                 \
        int test_##N(struct N *), ref_test_##N(struct N *);                \
        int zero_##N(struct N *), ref_zero_##N(struct N *);                \
        int neg_##N(struct N *), ref_neg_##N(struct N *);                  \
        void set_##N(struct N *, long), ref_set_##N(struct N *, long);     \
        long value_##N(struct N *, long), ref_value_##N(struct N *, long); \
        void clear_##N(struct N *), ref_clear_##N(struct N *);             \
        void fill_##N(struct N *), ref_fill_##N(struct N *);

#define CASE_DECLARE(N, T, W, PAD) SHAPES(DECLARE, N, T, W, PAD)

CASES(CASE_DECLARE)

int order_other(void), ref_order_other(void);
int order_same(void), ref_order_same(void);

/* the store S of record N on pattern k, by rcc and by clang, its bytes
   compared */
#define STORED(N, k, v, S, ARGUMENTS)           \
        fill(k, sizeof(struct N));              \
        S##_##N ARGUMENTS;                      \
        memcpy(saved, bytes, sizeof(struct N)); \
        fill(k, sizeof(struct N));              \
        ref_##S##_##N ARGUMENTS;                \
        report_bytes(#S "_" #N, k, v, saved, bytes, sizeof(struct N));

/* the reads and the stores of record N on each pattern and value */
#define CHECK(N, MEMBERS)                                                 \
        for (int k = 0; k < PATTERNS; ++k) {                              \
                struct N *p = (struct N *) bytes;                         \
                                                                          \
                fill(k, sizeof(struct N));                                \
                report("get_" #N, k, get_##N(p), ref_get_##N(p));         \
                report("test_" #N, k, test_##N(p), ref_test_##N(p));      \
                report("zero_" #N, k, zero_##N(p), ref_zero_##N(p));      \
                report("neg_" #N, k, neg_##N(p), ref_neg_##N(p));         \
                                                                          \
                for (int i = 0; i < VALUES; ++i) {                        \
                        const long v = values[i];                         \
                        long got;                                         \
                                                                          \
                        STORED(N, k, v, set, (p, v))                      \
                                                                          \
                        fill(k, sizeof(struct N));                        \
                        got = value_##N(p, v);                            \
                        memcpy(saved, bytes, sizeof(struct N));           \
                        fill(k, sizeof(struct N));                        \
                        report("value_" #N, k, got, ref_value_##N(p, v)); \
                        report_bytes("value_" #N, k, v, saved, bytes,     \
                                     sizeof(struct N));                   \
                }                                                         \
                                                                          \
                STORED(N, k, 0L, clear, (p))                              \
                STORED(N, k, -1L, fill, (p))                              \
        }

#define CASE_CHECK(N, T, W, PAD) SHAPES(CHECK, N, T, W, PAD)

int
main(void)
{
        CASES(CASE_CHECK)

        report("order_other", 0, order_other(), ref_order_other());
        report("order_same", 0, order_same(), ref_order_same());

        printf("%ld checks, %ld differ\n", total, bad);

        return bad != 0;
}
