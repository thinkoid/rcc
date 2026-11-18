/* The members probe: member access in rcc's C, docs/shape.md chapter
   4, on every member of the layout probe's records, a register pointer
   that walks a list, and a function pointer called through a member.
   See members.h. */

#include "members.h"

/* a call, which spills each value held across it */
long
held(long v)
{
        return v;
}

/* the functions of member M, of kind A, of record N declared by K: its
   scalar read, written, stepped, added to and its offset by ->, read by
   . on the second element of an array, written and added to there
   across a call, written and read on a local and on a global, and
   written, stepped and read by . on an element of an array by a
   variable index, through a pointer and on a global's label */
#define MEMBER(K, N, M, A)                                              \
    long get_##N##_##M(K N *p) { return (long) p->M PATH_##A; }         \
    void set_##N##_##M(K N *p, long v)                                  \
    {                                                                   \
        p->M PATH_##A = (SCALAR_##A) v;                                 \
    }                                                                   \
    long step_##N##_##M(K N *p) { return STEP_##A(p->M PATH_##A); }     \
    long add_##N##_##M(K N *p) { return ADD_##A(p->M PATH_##A); }       \
    long where_##N##_##M(K N *p)                                        \
    {                                                                   \
        return (char *) &p->M PATH_##A - (char *) p;                    \
    }                                                                   \
    long second_##N##_##M(K N *p) { return (long) p[1].M PATH_##A; }    \
    void held_##N##_##M(K N *p, int i, long v)                          \
    {                                                                   \
        p[i].M PATH_##A = (SCALAR_##A) held(v);                         \
    }                                                                   \
    long held_add_##N##_##M(K N *p, int i)                              \
    {                                                                   \
        return HELD_ADD_##A(p[i].M PATH_##A);                           \
    }                                                                   \
    long local_##N##_##M(long v)                                        \
    {                                                                   \
        K N s;                                                          \
                                                                        \
        s.M PATH_##A = (SCALAR_##A) v;                                  \
        return (long) s.M PATH_##A;                                     \
    }                                                                   \
    long global_##N##_##M(long v)                                       \
    {                                                                   \
        object_##N.M PATH_##A = (SCALAR_##A) v;                         \
        return (long) object_##N.M PATH_##A;                            \
    }                                                                   \
    long row_##N##_##M(K N *p, long i, long v)                          \
    {                                                                   \
        p[i].M PATH_##A = (SCALAR_##A) v;                               \
        (void) STEP_##A(p[i].M PATH_##A);                               \
        return (long) p[i].M PATH_##A;                                  \
    }                                                                   \
    long table_##N##_##M(long i, long v)                                \
    {                                                                   \
        table_##N[i].M PATH_##A = (SCALAR_##A) v;                       \
        (void) STEP_##A(table_##N[i].M PATH_##A);                       \
        return (long) table_##N[i].M PATH_##A;                          \
    }

#define PAIR_MEMBERS(A, TA, B, TB)                                      \
    struct p_##A##B object_p_##A##B;                                    \
    union u_##A##B object_u_##A##B;                                     \
    struct p_##A##B table_p_##A##B[3];                                  \
    union u_##A##B table_u_##A##B[3];                                   \
    MEMBER(struct, p_##A##B, x, A)                                      \
    MEMBER(struct, p_##A##B, y, B)                                      \
    MEMBER(union, u_##A##B, x, A)                                       \
    MEMBER(union, u_##A##B, y, B)

#define PAIRS_MEMBERS(A, TA) KINDS2(PAIR_MEMBERS, A, TA)

KINDS(PAIRS_MEMBERS)

#define TRIPLE_MEMBERS(A, TA, B, TB, C, TC)                             \
    struct t_##A##B##C object_t_##A##B##C;                              \
    struct t_##A##B##C table_t_##A##B##C[3];                            \
    MEMBER(struct, t_##A##B##C, x, A)                                   \
    MEMBER(struct, t_##A##B##C, y, B)                                   \
    MEMBER(struct, t_##A##B##C, z, C)

#define TRIPLES_OF_MEMBERS(A, TA, B, TB) FEW3(TRIPLE_MEMBERS, A, TA, B, TB)
#define TRIPLES_MEMBERS(A, TA) FEW2(TRIPLES_OF_MEMBERS, A, TA)

FEW(TRIPLES_MEMBERS)

/* a register pointer that walks the list, r = r->next: the variable is
   read and written through its own member, chapter 4 of backend.md */
long
walk(struct link *p)
{
        register struct link *r = p;
        long n = 0;

        while (r) {
                n += r->value;
                r = r->next;
        }

        return n;
}

/* two links a step, each read through the variable */
long
walk_pairs(struct link *p)
{
        register struct link *r = p;
        long n = 0;

        while (r && r->next) {
                n = n * 3 + r->next->value - r->value;
                r = r->next->next;
        }

        return n;
}

/* the walk in a for, the variable stepped by its own member */
long
walk_count(struct link *p)
{
        register struct link *r;
        long n = 0;

        for (r = p; r; r = r->next)
                ++n;

        return n;
}

/* a function pointer called through a member, by -> and by . */
int
apply(struct ops *o, int x)
{
        return o->twice(x) + o->bias + (*o).twice(o->bias);
}

/* a row by a variable index on a label, at an offset no multiple of
   the access's size, whose label's address the access keeps */
struct link odd_links[4];

long
odd_row(long i, long v)
{
        *(int *) ((char *) &odd_links[i] + 2) = (int) v;
        *(int *) ((char *) &odd_links[i] + 2) += 5;
        return odd_links[i].value + *(short *) ((char *) &odd_links[i] + 6);
}
