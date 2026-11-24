/* The fields probe: the read and the store of a bit-field in rcc's C,
   docs/shape.md chapter 5.  Per record of fields.h, its field's value,
   the field as a condition, compared with 0, and its value less 1
   compared with 0, which shows its promoted type; a value stored into
   it, the value of that store, and the stores of 0 and of -1; on the
   field alone, each compound assignment and each step, as a statement
   and for its value, and beside a char, += for its value and a
   postfix step as a statement.  Then the order of a store and of a compound assignment
   whose right operand stores into the same unit, and a store, a
   compound assignment and a step through a place with a side effect.
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

/* the compound assignment NAME of the field of record N, as a
   statement and for its value */
#define COMPOUND(N, NAME, OP, RIGHT, TAKEN)        \
        void NAME##_##N(struct N *p, long v)       \
        {                                          \
                p->f OP RIGHT;                     \
        }                                          \
                                                   \
        long NAME##_value_##N(struct N *p, long v) \
        {                                          \
                return p->f OP RIGHT;              \
        }

/* the step NAME of the field of record N, as a statement and for its
   value */
#define STEP(N, NAME, STEPPED)             \
        void NAME##_##N(struct N *p)       \
        {                                  \
                STEPPED;                   \
        }                                  \
                                           \
        long NAME##_value_##N(struct N *p) \
        {                                  \
                return STEPPED;            \
        }

#define UPDATES(N, MEMBERS) COMPOUNDS(COMPOUND, N) STEPS(STEP, N)

/* the bytes beside the field of record N, written back through the
   temporary: += for its value, and a postfix step as a statement */
#define WRITTEN_BACK(N, MEMBERS)                \
        long add_value_##N(struct N *p, long v) \
        {                                       \
                return p->f += v;               \
        }                                       \
                                                \
        void postinc_##N(struct N *p)           \
        {                                       \
                p->f++;                         \
        }

/* every compound assignment and step on the field alone; the bytes
   written back on the two shapes with a char beside it */
#define CASE_UPDATES(N, T, W, PAD) \
        ALONE(UPDATES, N, T, W, PAD) BESIDE(WRITTEN_BACK, N, T, W, PAD)

CASES(CASE_UPDATES)

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

/* the order of a compound assignment: its right operand calls the
   same functions, each before the unit is loaded */
int
order_compound_other(void)
{
        order.a = 0;
        order.f = 2;
        order.f += order_other_field();
        return order.a * 100 + order.f;
}

int
order_compound_same(void)
{
        order.a = 3;
        order.f = 0;
        order.f += order_same_field();
        return order.a * 100 + order.f;
}

/* a store, a compound assignment and a step through a place with a
   side effect, on the field f of an element of an array of struct
   order: the side effect happens once, the element's other field
   stays, and so do the elements beside it */
struct order line[3];

/* the fields of line, after the count k, in one value */
long
line_state(long k)
{
        long s = k;

        for (int i = 0; i < 3; ++i)
                s = (s * 8 + line[i].a) * 32 + (line[i].f & 31);

        return s;
}

void
line_fill(void)
{
        for (int i = 0; i < 3; ++i) {
                line[i].a = i + 2;
                line[i].f = -3 * i - 1;
        }
}

long
line_store(long v)
{
        int i = 1;

        line_fill();
        line[i++].f = v;

        return line_state(i);
}

int line_calls;

struct order *
line_middle(void)
{
        ++line_calls;
        return &line[1];
}

long
line_compound(long v)
{
        line_calls = 0;
        line_fill();
        line_middle()->f += v;

        return line_state(line_calls);
}

long
line_step(void)
{
        struct order *p = &line[1];
        long before;

        line_fill();
        before = p++->f++;

        return line_state(p - line) * 64 + (before & 63);
}
