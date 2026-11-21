/* The lists the layout probe is generated from.  layout.c makes the
   functions of each record, compiled by rcc and, as the reference, by
   clang -O0 under the prefix ref_; layout-main.c calls both and
   compares.  The records are defined here, so the main, rcc's build and
   clang's see one set: a struct and a union of each pair of member
   kinds, a struct of each triple of a smaller set, and records that
   hold records.  The floating kinds, a real floating type or an array
   of double, which a record lays out though rcc uses no floating
   object, are a list apart: each pairs with every kind and stands
   between two of the smaller set, and the members probe, which reads
   each member of the other lists, leaves them out.  The layout is the parser's, docs/shape.md chapter 3,
   and each function returns what the parser folds, a size, an
   alignment or an offset, but for the stride of an array of the record,
   which c1 computes from the record's size, and for the place of an
   object of the record, a local's and a global's, which c1 aligns.
   struct sockaddr_in is a header's record, whose may_alias attribute
   rcc drops.  va_list is the target's list, 7.16p4, an array of one
   record on x86-64 and a record on aarch64, whose members are read
   through the list's address.  struct file_handle is a header's record
   under _GNU_SOURCE, whose last member glibc writes as an array of size
   zero, a flexible array member.  struct termios is a header's record
   whose last two members glibc writes as anonymous unions. */

#define _GNU_SOURCE

#include <fcntl.h>
#include <netinet/in.h>
#include <stdarg.h>
#include <stddef.h>
#include <termios.h>

/* the member kinds that are no record, spelled as one type each */
typedef char *pointer_t;
typedef int (*function_t)(int);
typedef char chars_t[3];
typedef short shorts_t[3];
typedef double doubles_t[2];

/* the records that stand as members: a struct whose second member is
   aligned past its first, a union of two widths, and an array of the
   struct */
struct inner
{
        char c;
        long l;
};

union either
{
        char c;
        int i;
};

typedef struct inner inner_t;
typedef union either either_t;
typedef inner_t inners_t[2];

/* the member kinds: a letter, for the record's name, and the type */
/* clang-format off */
#define KINDS(X)                                                        \
    X(c, char) X(s, short) X(i, int) X(l, long) X(p, pointer_t)         \
    X(f, function_t) X(a, chars_t) X(h, shorts_t) X(r, inner_t)         \
    X(u, either_t) X(n, inners_t)

/* the same list under a second name, for the pairs */
#define KINDS2(X, A, TA)                                                \
    X(A, TA, c, char) X(A, TA, s, short) X(A, TA, i, int)               \
    X(A, TA, l, long) X(A, TA, p, pointer_t) X(A, TA, f, function_t)    \
    X(A, TA, a, chars_t) X(A, TA, h, shorts_t) X(A, TA, r, inner_t)     \
    X(A, TA, u, either_t) X(A, TA, n, inners_t)

/* a smaller set for the triples, under a first, a second and a third
   name */
#define FEW(X)                                                          \
    X(c, char) X(s, short) X(l, long) X(a, chars_t) X(r, inner_t)       \
    X(u, either_t)

#define FEW2(X, A, TA)                                                  \
    X(A, TA, c, char) X(A, TA, s, short) X(A, TA, l, long)              \
    X(A, TA, a, chars_t) X(A, TA, r, inner_t) X(A, TA, u, either_t)

#define FEW3(X, A, TA, B, TB)                                           \
    X(A, TA, B, TB, c, char) X(A, TA, B, TB, s, short)                  \
    X(A, TA, B, TB, l, long) X(A, TA, B, TB, a, chars_t)                \
    X(A, TA, B, TB, r, inner_t) X(A, TA, B, TB, u, either_t)

/* the floating kinds, under a first and a second name */
#define FLOATING(X)                                                     \
    X(g, float) X(d, double) X(q, long double) X(v, doubles_t)

#define FLOATING2(X, A, TA)                                             \
    X(A, TA, g, float) X(A, TA, d, double) X(A, TA, q, long double)     \
    X(A, TA, v, doubles_t)
/* clang-format on */

/* the pairs: struct p_AB and union u_AB, members x and y */
#define PAIR(A, TA, B, TB) \
        struct p_##A##B    \
        {                  \
                TA x;      \
                TB y;      \
        };                 \
        union u_##A##B     \
        {                  \
                TA x;      \
                TB y;      \
        };

#define PAIRS(A, TA) KINDS2(PAIR, A, TA)

KINDS(PAIRS)

/* the triples: struct t_ABC, members x, y and z */
#define TRIPLE(A, TA, B, TB, C, TC) \
        struct t_##A##B##C          \
        {                           \
                TA x;               \
                TB y;               \
                TC z;               \
        };

#define TRIPLES_OF(A, TA, B, TB) FEW3(TRIPLE, A, TA, B, TB)
#define TRIPLES(A, TA)           FEW2(TRIPLES_OF, A, TA)

FEW(TRIPLES)

/* the pairs of a floating kind and any kind, either way round, and of
   two floating kinds; the triples of a floating kind between two of
   the smaller set */
#define FLOATING_PAIRS(A, TA) FLOATING2(PAIR, A, TA)

KINDS(FLOATING_PAIRS)
FLOATING(PAIRS)
FLOATING(FLOATING_PAIRS)

#define FLOATING_TRIPLES_OF(A, TA, B, TB) FEW3(TRIPLE, A, TA, B, TB)
#define FLOATING_TRIPLES(A, TA)           FLOATING2(FLOATING_TRIPLES_OF, A, TA)

FEW(FLOATING_TRIPLES)

/* records of records: a struct of a pair and of an array of triples,
   whose designators reach into both; a union of two structs; and a
   struct that declares an enumeration and checks a static assertion in
   its list, whose constants take the scope the struct is declared in,
   6.2.1p4 */
struct nest
{
        char c;
        struct p_cl pair;
        struct t_rsa triples[2];
        union u_ri either;
};

union choice
{
        struct p_lc a;
        struct t_cac b;
};

struct tagged
{
        enum mode { M0, M1, M2 } mode;
        char c;
        _Static_assert(sizeof(struct p_lc) == 16, "p_lc");
};

/* records whose last member is a flexible array member, 6.7.3.2p20,
   which stands at its element's alignment past the other members and
   adds no size: of chars after an int, of ints after a char, of longs
   after a char and a short, and of records after a short */
struct flexible_c
{
        int n;
        unsigned char b[];
};

struct flexible_i
{
        char c;
        int b[];
};

struct flexible_l
{
        char c;
        short s;
        long b[];
};

struct flexible_r
{
        short s;
        struct inner b[];
};

/* records of anonymous members, 6.7.3.2p15, whose members are the
   enclosing record's, laid out as the anonymous member's own record: a
   union of a char and a long between a char and a short, a struct of a
   char and a long in a union, the union of the EXAMPLE 2 there, of an
   anonymous struct beside a named one, and the struct of its EXAMPLE 4,
   an anonymous struct's member and a flexible array member */
struct anonymous_union
{
        char c;
        union
        {
                char a;
                long l;
        };
        short s;
};

union anonymous_struct
{
        int i;
        struct
        {
                char c;
                long l;
        };
};

struct anonymous_nest
{
        union
        {
                struct
                {
                        int i, j;
                };
                struct
                {
                        long k, l;
                } w;
        };
        int m;
};

struct anonymous_flexible
{
        struct
        {
                int i;
        };
        int a[];
};

/* the records of no list above, and the keyword each is declared by */
/* clang-format off */
#define RECORDS(X)                                                      \
    X(struct, inner) X(union, either) X(struct, nest) X(union, choice)  \
    X(struct, tagged) X(struct, sockaddr_in) X(, va_list)               \
    X(struct, anonymous_union) X(union, anonymous_struct)               \
    X(struct, anonymous_nest) X(struct, termios)

/* the records that hold a flexible array member, which are no element
   of an array, 6.7.3.2p3 */
#define FLEXIBLE(X)                                                     \
    X(struct, flexible_c) X(struct, flexible_i) X(struct, flexible_l)   \
    X(struct, flexible_r) X(struct, file_handle)                        \
    X(struct, anonymous_flexible)

/* the designators of offsetof into those records: a name, the record,
   the designator */
#define DESIGNATORS(X)                                                  \
    X(inner_l, struct inner, l) X(nest_pair_y, struct nest, pair.y)     \
    X(nest_triple_1_z, struct nest, triples[1].z)                       \
    X(nest_triple_1_x_l, struct nest, triples[1].x.l)                   \
    X(nest_either_x_l, struct nest, either.x.l)                         \
    X(choice_b_z, union choice, b.z) X(tagged_c, struct tagged, c)     \
    X(flexible_c_b, struct flexible_c, b)                               \
    X(flexible_i_b, struct flexible_i, b)                               \
    X(flexible_l_b, struct flexible_l, b)                               \
    X(flexible_r_b, struct flexible_r, b)                               \
    X(flexible_r_b_1_l, struct flexible_r, b[1].l)                      \
    X(file_handle_f_handle, struct file_handle, f_handle)               \
    X(anonymous_union_a, struct anonymous_union, a)                     \
    X(anonymous_union_l, struct anonymous_union, l)                     \
    X(anonymous_union_s, struct anonymous_union, s)                     \
    X(anonymous_struct_c, union anonymous_struct, c)                    \
    X(anonymous_struct_l, union anonymous_struct, l)                    \
    X(anonymous_nest_j, struct anonymous_nest, j)                       \
    X(anonymous_nest_w_l, struct anonymous_nest, w.l)                   \
    X(anonymous_nest_m, struct anonymous_nest, m)                       \
    X(anonymous_flexible_a, struct anonymous_flexible, a)               \
    X(termios_c_cc, struct termios, c_cc)                               \
    X(termios_c_ispeed, struct termios, c_ispeed)                       \
    X(termios_c_ospeed, struct termios, c_ospeed)
/* clang-format on */

/* the members of the target's list record, and the list's address */
#if defined(__x86_64__)
/* clang-format off */
#define LIST_MEMBERS(X)                                                 \
    X(gp_offset) X(fp_offset) X(overflow_arg_area) X(reg_save_area)
/* clang-format on */
#  define LIST(L) (L)
#else
/* clang-format off */
#define LIST_MEMBERS(X)                                                 \
    X(__stack) X(__gr_top) X(__vr_top) X(__gr_offs) X(__vr_offs)
/* clang-format on */
#  define LIST(L) (&(L))
#endif
