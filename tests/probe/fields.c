/* The fields probe: the read and the store of a bit-field in rcc's C,
   docs/shape.md chapter 5.  Per record of fields.h, its field's value,
   the field as a condition, compared with 0, and its value less 1
   compared with 0, which shows its promoted type; a value stored into
   it, the value of that store, and the stores of 0 and of -1.  Then
   the order of a store whose right operand stores into the same unit.
   See fields.h. */

#include "fields.h"

/* the reads and the stores of the field of record N */
#define READS(N, MEMBERS)                   \
        long get_##N(struct N *p)           \
        {                                   \
                return p->f;                \
        }                                   \
                                            \
        int test_##N(struct N *p)           \
        {                                   \
                if (p->f)                   \
                        return 1;           \
                return 0;                   \
        }                                   \
                                            \
        int zero_##N(struct N *p)           \
        {                                   \
                return p->f == 0;           \
        }                                   \
                                            \
        int neg_##N(struct N *p)            \
        {                                   \
                return p->f - 1 < 0;        \
        }                                   \
                                            \
        void set_##N(struct N *p, long v)   \
        {                                   \
                p->f = v;                   \
        }                                   \
                                            \
        long value_##N(struct N *p, long v) \
        {                                   \
                return p->f = v;            \
        }                                   \
                                            \
        void clear_##N(struct N *p)         \
        {                                   \
                p->f = 0;                   \
        }                                   \
                                            \
        void fill_##N(struct N *p)          \
        {                                   \
                p->f = -1;                  \
        }

#define CASE_READS(N, T, W, PAD) SHAPES(READS, N, T, W, PAD)

CASES(CASE_READS)

/* the order of a store: the right operand calls a function that stores
   into the other field of the unit, and one that stores into the same
   field, each before the unit is loaded */
struct order order;

int
order_other_field(void)
{
        order.a = 5;
        return 1;
}

int
order_other(void)
{
        order.a = 0;
        order.f = 0;
        order.f = order_other_field();
        return order.a * 100 + order.f;
}

int
order_same_field(void)
{
        order.f = 7;
        return 2;
}

int
order_same(void)
{
        order.a = 3;
        order.f = 0;
        order.f = order_same_field();
        return order.a * 100 + order.f;
}
