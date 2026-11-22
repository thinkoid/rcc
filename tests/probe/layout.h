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
#include <fenv.h>
#include <netinet/in.h>
#include <regex.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/timex.h>
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

/* The bit-fields, 6.7.3.2p11, laid out as clang lays them out on the
   target, docs/shape.md chapter 3.  A bit-field has no offset in bytes,
   so the functions of a record of bit-fields are those of any record
   and the offsets of its ordinary members, which show where the fields
   before them end. */

/* the enumerated type of a bit-field, of unsigned int */
enum two_bits { TB0, TB1, TB2, TB3 };

/* the bit-fields: a name, the declared type and the width; three widths
   of each integer type, 1, one past its half and its own, and those of
   _Bool and of an enumerated type */
/* clang-format off */
#define FIELDS(X)                                                       \
    X(c1, char, 1) X(c5, char, 5) X(c8, char, 8)                        \
    X(a1, signed char, 1) X(a5, signed char, 5) X(a8, signed char, 8)   \
    X(h1, unsigned char, 1) X(h5, unsigned char, 5)                     \
    X(h8, unsigned char, 8)                                             \
    X(s1, short, 1) X(s9, short, 9) X(s16, short, 16)                   \
    X(t1, unsigned short, 1) X(t9, unsigned short, 9)                   \
    X(t16, unsigned short, 16)                                          \
    X(i1, int, 1) X(i17, int, 17) X(i32, int, 32)                       \
    X(u1, unsigned, 1) X(u17, unsigned, 17) X(u32, unsigned, 32)        \
    X(l1, long, 1) X(l33, long, 33) X(l64, long, 64)                    \
    X(m1, unsigned long, 1) X(m33, unsigned long, 33)                   \
    X(m64, unsigned long, 64)                                           \
    X(q1, long long, 1) X(q33, long long, 33) X(q64, long long, 64)     \
    X(r1, unsigned long long, 1) X(r33, unsigned long long, 33)         \
    X(r64, unsigned long long, 64)                                      \
    X(b1, _Bool, 1) X(e2, enum two_bits, 2) X(e32, enum two_bits, 32)

/* the member kinds after a bit-field's name, type and width */
#define FIELD_KINDS(X, F, TF, WF)                                       \
    X(F, TF, WF, c, char) X(F, TF, WF, s, short) X(F, TF, WF, i, int)   \
    X(F, TF, WF, l, long) X(F, TF, WF, p, pointer_t)                    \
    X(F, TF, WF, f, function_t) X(F, TF, WF, a, chars_t)                \
    X(F, TF, WF, h, shorts_t) X(F, TF, WF, r, inner_t)                  \
    X(F, TF, WF, u, either_t) X(F, TF, WF, n, inners_t)

/* a smaller set of bit-fields for the pairs of two, under a first and a
   second name */
#define FEW_FIELDS(X)                                                   \
    X(c5, char, 5) X(h8, unsigned char, 8) X(s9, short, 9)              \
    X(u17, unsigned, 17) X(l33, long, 33) X(b1, _Bool, 1)

#define FEW_FIELDS2(X, A, TA, WA)                                       \
    X(A, TA, WA, c5, char, 5) X(A, TA, WA, h8, unsigned char, 8)        \
    X(A, TA, WA, s9, short, 9) X(A, TA, WA, u17, unsigned, 17)          \
    X(A, TA, WA, l33, long, 33) X(A, TA, WA, b1, _Bool, 1)
/* clang-format on */

/* a bit-field and a member of each kind, either way round: struct fk_FK,
   members f and y, and struct kf_FK, members x and f */
#define FIELD_PAIR(F, TF, WF, K, TK) \
        struct fk_##F##_##K          \
        {                            \
                TF f : WF;           \
                TK y;                \
        };                           \
        struct kf_##F##_##K          \
        {                            \
                TK x;                \
                TF f : WF;           \
        };

#define FIELD_PAIRS(F, TF, WF) FIELD_KINDS(FIELD_PAIR, F, TF, WF)

FIELDS(FIELD_PAIRS)

/* two bit-fields, of one type or of two, and a char after them: struct
   ff_AB, members x, y and z */
#define FIELD_TWO(A, TA, WA, B, TB, WB) \
        struct ff_##A##_##B             \
        {                               \
                TA x : WA;              \
                TB y : WB;              \
                char z;                 \
        };

#define FIELD_TWOS(A, TA, WA) FEW_FIELDS2(FIELD_TWO, A, TA, WA)

FEW_FIELDS(FIELD_TWOS)

/* the rules of the layout, one record or more each: a field in the rest
   of its container or in the next one, never across two; fields from
   the lowest bit up; fields of different types packed while each fits
   its own container; an ordinary member at the next byte past the
   fields, in a field's container; a width of 0 to the next container
   boundary of its type; the field's signedness, which moves nothing;
   the alignment of a named field's type and of an unnamed one's, which
   clang counts on aarch64 alone; a union's fields at bit 0 */
struct rule_flags
{
        unsigned a : 1;
        unsigned b : 3;
};

struct rule_straddle
{
        int a : 30;
        int b : 4;
        char c;
};

struct rule_after_char
{
        char c;
        int f : 4;
        char d;
};

struct rule_after_char_28
{
        char c;
        int f : 28;
        char d;
};

struct rule_after_char_24
{
        char c;
        int f : 24;
        char d;
};

struct rule_unit_in_container
{
        char c;
        int f : 16;
        char d;
};

struct rule_long
{
        long a : 40;
        long b : 30;
        char c;
};

struct rule_shorts
{
        short a : 9, b : 9;
        char c;
};

struct rule_after_chars
{
        char c[3];
        int f : 9;
        char d;
};

struct rule_low_first
{
        int a : 3, b : 5;
        char c;
};

struct rule_mixed
{
        unsigned char a : 3;
        unsigned short b : 7;
        unsigned c : 9;
        char d;
};

struct rule_int_long
{
        int a : 20;
        long b : 40;
        char c;
};

struct rule_long_int
{
        long a : 40;
        int b : 20;
        char c;
};

struct rule_char_long_long
{
        char c;
        long long f : 33;
        char d;
};

struct rule_short_between
{
        char a;
        short b : 4;
        char c;
};

struct rule_zero_int
{
        int a : 3;
        int : 0;
        int b : 3;
        char c;
};

struct rule_zero_char
{
        int a : 3;
        char : 0;
        int b : 3;
        char c;
};

struct rule_signedness
{
        int a : 3;
        unsigned b : 3;
        signed c : 3;
        char d;
};

struct rule_enum
{
        enum two_bits e : 2;
        char c;
};

struct rule_bools
{
        _Bool a : 1, b : 1;
        char c;
};

struct rule_chars
{
        char a : 4, b : 4, c : 4;
        char d;
};

struct rule_named_long
{
        char c;
        long f : 1;
};

struct rule_unnamed_4
{
        char c;
        int : 4;
        char d;
};

struct rule_unnamed_0
{
        char c;
        int : 0;
        char d;
};

struct rule_unnamed_long
{
        char c;
        long : 1;
};

struct rule_unnamed_first
{
        int : 0;
        char c;
};

struct rule_unnamed_last
{
        char c;
        int : 4;
};

struct rule_unnamed_short
{
        char c;
        short : 0;
        char d;
};

struct rule_unnamed_long_first
{
        long : 0;
        char c;
};

struct rule_unnamed_between
{
        char a : 3;
        int : 0;
        char b : 3;
};

union rule_union_field
{
        int a : 3;
        char c;
};

union rule_union_unnamed
{
        char c;
        int : 3;
};

union rule_union_long
{
        long a : 3;
};

union rule_union_after
{
        char c;
        int a : 3;
};

union rule_union_zero
{
        char c;
        int : 0;
};

/* records in the shape of the C library's: FILE's _flags2 between an int
   and a char of its container, fenv_t's opcode after a short on x86-64,
   and the shell's struct job, whose fields fill a word between an int
   and a pointer; and a struct of fields of three types before a long
   field */
struct like_file
{
        int fileno;
        int flags2 : 24;
        char backup[1];
        long off;
};

struct like_fenv
{
        unsigned short cs;
        unsigned op : 11;
        unsigned r : 5;
        unsigned d;
};

struct like_job
{
        int stop;
        unsigned nprocs : 16, state : 8, sigint : 1, jobctl : 1, waited : 1,
            used : 1, changed : 1;
        struct like_job *prev;
};

struct like_g
{
        unsigned a : 3;
        int f : 5;
        unsigned u : 7;
        long l : 40;
        char c;
};

/* bit-fields in an anonymous struct, whose members are the record's,
   6.7.3.2p15, and an anonymous struct of a bit-field alone before a
   flexible array member, which it counts as a named member before, p3 */
struct anonymous_fields
{
        char c;
        struct
        {
                int a : 3;
                char d;
                int b : 5;
        };
        short s;
};

struct anonymous_field_flexible
{
        struct
        {
                int b : 3;
        };
        int a[];
};

/* the records of bit-fields of no list above, the C library's among
   them, and the keyword each is declared by */
/* clang-format off */
#define FIELD_RECORDS(X)                                                \
    X(struct, rule_flags) X(struct, rule_straddle)                      \
    X(struct, rule_after_char)                                          \
    X(struct, rule_after_char_28) X(struct, rule_after_char_24)         \
    X(struct, rule_unit_in_container) X(struct, rule_long)              \
    X(struct, rule_shorts) X(struct, rule_after_chars)                  \
    X(struct, rule_low_first) X(struct, rule_mixed)                     \
    X(struct, rule_int_long) X(struct, rule_long_int)                   \
    X(struct, rule_char_long_long) X(struct, rule_short_between)        \
    X(struct, rule_zero_int) X(struct, rule_zero_char)                  \
    X(struct, rule_signedness) X(struct, rule_enum)                     \
    X(struct, rule_bools) X(struct, rule_chars)                         \
    X(struct, rule_named_long) X(struct, rule_unnamed_4)                \
    X(struct, rule_unnamed_0) X(struct, rule_unnamed_long)              \
    X(struct, rule_unnamed_first) X(struct, rule_unnamed_last)          \
    X(struct, rule_unnamed_short) X(struct, rule_unnamed_long_first)    \
    X(struct, rule_unnamed_between) X(union, rule_union_field)          \
    X(union, rule_union_unnamed) X(union, rule_union_long)              \
    X(union, rule_union_after) X(union, rule_union_zero)                \
    X(struct, like_file) X(struct, like_fenv) X(struct, like_job)       \
    X(struct, like_g) X(struct, anonymous_fields) X(, FILE)             \
    X(, regex_t) X(, fenv_t) X(struct, timex)

/* the ordinary members of those records, and of the flexible one: the
   keyword, the record, the member */
#define FIELD_OFFSETS(X)                                                \
    X(struct, rule_straddle, c) X(struct, rule_after_char, c)           \
    X(struct, rule_after_char, d) X(struct, rule_after_char_28, d)      \
    X(struct, rule_after_char_24, d)                                    \
    X(struct, rule_unit_in_container, d) X(struct, rule_long, c)        \
    X(struct, rule_shorts, c) X(struct, rule_after_chars, d)            \
    X(struct, rule_low_first, c) X(struct, rule_mixed, d)               \
    X(struct, rule_int_long, c) X(struct, rule_long_int, c)             \
    X(struct, rule_char_long_long, d)                                   \
    X(struct, rule_short_between, c) X(struct, rule_zero_int, c)        \
    X(struct, rule_zero_char, c) X(struct, rule_signedness, d)          \
    X(struct, rule_enum, c) X(struct, rule_bools, c)                    \
    X(struct, rule_chars, d) X(struct, rule_unnamed_4, d)               \
    X(struct, rule_unnamed_0, d) X(struct, rule_unnamed_first, c)       \
    X(struct, rule_unnamed_short, d)                                    \
    X(struct, rule_unnamed_long_first, c) X(union, rule_union_field, c) \
    X(union, rule_union_unnamed, c) X(union, rule_union_after, c)       \
    X(union, rule_union_zero, c) X(struct, like_file, backup)           \
    X(struct, like_file, off) X(struct, like_fenv, d)                   \
    X(struct, like_job, prev) X(struct, like_g, c)                      \
    X(struct, anonymous_fields, d) X(struct, anonymous_fields, s)       \
    X(struct, anonymous_field_flexible, a)

/* the ordinary members of the C library's records, by offsetof of a
   type name: a name, the type, the member */
#define LIBRARY_OFFSETS(X)                                              \
    X(file_fileno, FILE, _fileno)                                       \
    X(file_short_backupbuf, FILE, _short_backupbuf)                     \
    X(file_old_offset, FILE, _old_offset)                               \
    X(file_lock, FILE, _lock)                                           \
    X(regex_re_nsub, regex_t, re_nsub)                                  \
    X(regex_translate, regex_t, translate)
/* clang-format on */
