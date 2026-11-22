/* The fields probe: the read of a bit-field in rcc's C, docs/shape.md
   chapter 5.  Per record of fields.h, its field's value, the field as
   a condition, compared with 0, and its value less 1 compared with 0,
   which shows its promoted type.  See fields.h. */

#include "fields.h"

/* the reads of the field of record N */
#define READS(N, MEMBERS)            \
        long get_##N(struct N *p)    \
        {                            \
                return p->f;         \
        }                            \
                                     \
        int test_##N(struct N *p)    \
        {                            \
                if (p->f)            \
                        return 1;    \
                return 0;            \
        }                            \
                                     \
        int zero_##N(struct N *p)    \
        {                            \
                return p->f == 0;    \
        }                            \
                                     \
        int neg_##N(struct N *p)     \
        {                            \
                return p->f - 1 < 0; \
        }

#define CASE_READS(N, T, W, PAD) SHAPES(READS, N, T, W, PAD)

CASES(CASE_READS)
