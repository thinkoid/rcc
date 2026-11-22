/* The layout probe: the size, the alignment and the member offsets of
   structs and unions, in rcc's C, docs/shape.md chapter 3, the stride
   of an array of each, which c1 computes from the size, and the place
   of an object of each.  See layout.h. */

#include "layout.h"

/* the functions every record has: its size, its alignment, the size of
   an array of three, the distance from an element's address to the
   next one's, and whether a local and a global of it, each after a char
   that would leave it at an odd address, are aligned: 0 when both are */
#define RECORD(K, N)                                                       \
        long size_##N(void)                                                \
        {                                                                  \
                return sizeof(K N);                                        \
        }                                                                  \
        long align_##N(void)                                               \
        {                                                                  \
                return _Alignof(K N);                                      \
        }                                                                  \
        long array_##N(void)                                               \
        {                                                                  \
                return sizeof(K N[3]);                                     \
        }                                                                  \
        long stride_##N(K N *p)                                            \
        {                                                                  \
                return (char *) &p[1] - (char *) p;                        \
        }                                                                  \
        char before_##N;                                                   \
        K N global_##N;                                                    \
        long place_##N(void)                                               \
        {                                                                  \
                char c;                                                    \
                K N local;                                                 \
                                                                           \
                return ((unsigned long) &local % _Alignof(K N) != 0) * 2 + \
                       ((unsigned long) &global_##N % _Alignof(K N) != 0); \
        }

/* the offset of a member */
#define OFFSET(K, N, M)                  \
        long offset_##N##_##M(void)      \
        {                                \
                return offsetof(K N, M); \
        }

#define PAIR_FUNCTIONS(A, TA, B, TB) \
        RECORD(struct, p_##A##B)     \
        OFFSET(struct, p_##A##B, x)  \
        OFFSET(struct, p_##A##B, y)  \
        RECORD(union, u_##A##B)      \
        OFFSET(union, u_##A##B, x)   \
        OFFSET(union, u_##A##B, y)

#define PAIRS_FUNCTIONS(A, TA) KINDS2(PAIR_FUNCTIONS, A, TA)

KINDS(PAIRS_FUNCTIONS)

#define TRIPLE_FUNCTIONS(A, TA, B, TB, C, TC) \
        RECORD(struct, t_##A##B##C)           \
        OFFSET(struct, t_##A##B##C, x)        \
        OFFSET(struct, t_##A##B##C, y)        \
        OFFSET(struct, t_##A##B##C, z)

#define TRIPLES_OF_FUNCTIONS(A, TA, B, TB) FEW3(TRIPLE_FUNCTIONS, A, TA, B, TB)
#define TRIPLES_FUNCTIONS(A, TA)           FEW2(TRIPLES_OF_FUNCTIONS, A, TA)

FEW(TRIPLES_FUNCTIONS)

#define FLOATING_PAIRS_FUNCTIONS(A, TA) FLOATING2(PAIR_FUNCTIONS, A, TA)

KINDS(FLOATING_PAIRS_FUNCTIONS)
FLOATING(PAIRS_FUNCTIONS)
FLOATING(FLOATING_PAIRS_FUNCTIONS)

#define FLOATING_TRIPLES_OF_FUNCTIONS(A, TA, B, TB) \
        FEW3(TRIPLE_FUNCTIONS, A, TA, B, TB)
#define FLOATING_TRIPLES_FUNCTIONS(A, TA) \
        FLOATING2(FLOATING_TRIPLES_OF_FUNCTIONS, A, TA)

FEW(FLOATING_TRIPLES_FUNCTIONS)

RECORDS(RECORD)

/* the functions of a record that holds a flexible array member, which
   is no element of an array, 6.7.3.2p3: its size, its alignment, and
   whether a local and a global of it are aligned, as above */
#define FLEXIBLE_RECORD(K, N)                                              \
        long size_##N(void)                                                \
        {                                                                  \
                return sizeof(K N);                                        \
        }                                                                  \
        long align_##N(void)                                               \
        {                                                                  \
                return _Alignof(K N);                                      \
        }                                                                  \
        char before_##N;                                                   \
        K N global_##N;                                                    \
        long place_##N(void)                                               \
        {                                                                  \
                char c;                                                    \
                K N local;                                                 \
                                                                           \
                return ((unsigned long) &local % _Alignof(K N) != 0) * 2 + \
                       ((unsigned long) &global_##N % _Alignof(K N) != 0); \
        }

FLEXIBLE(FLEXIBLE_RECORD)

/* offsetof by a designator that goes into the members' records */
#define DESIGNATED(N, R, D)            \
        long designated_##N(void)      \
        {                              \
                return offsetof(R, D); \
        }

DESIGNATORS(DESIGNATED)

/* the offset of a member of the list's record, from the list's address */
#define LIST_OFFSET(M)                                          \
        long list_##M(void)                                     \
        {                                                       \
                va_list l;                                      \
                                                                \
                return (char *) &LIST(l)->M - (char *) LIST(l); \
        }

LIST_MEMBERS(LIST_OFFSET)

/* an enumeration constant the member list of struct tagged declares */
int
mode_two(void)
{
        return M2;
}

/* the functions of the records of bit-fields: a bit-field and a member
   of each kind, either way round; two bit-fields and a char; the rules
   and the C library's records; and the offsets of their ordinary
   members, since a bit-field has none */
#define FIELD_PAIR_FUNCTIONS(F, TF, WF, K, TK) \
        RECORD(struct, fk_##F##_##K)           \
        OFFSET(struct, fk_##F##_##K, y)        \
        RECORD(struct, kf_##F##_##K)           \
        OFFSET(struct, kf_##F##_##K, x)

#define FIELD_PAIRS_FUNCTIONS(F, TF, WF) \
        FIELD_KINDS(FIELD_PAIR_FUNCTIONS, F, TF, WF)

FIELDS(FIELD_PAIRS_FUNCTIONS)

#define FIELD_TWO_FUNCTIONS(A, TA, WA, B, TB, WB) \
        RECORD(struct, ff_##A##_##B)              \
        OFFSET(struct, ff_##A##_##B, z)

#define FIELD_TWOS_FUNCTIONS(A, TA, WA) \
        FEW_FIELDS2(FIELD_TWO_FUNCTIONS, A, TA, WA)

FEW_FIELDS(FIELD_TWOS_FUNCTIONS)

FIELD_RECORDS(RECORD)
FLEXIBLE_RECORD(struct, anonymous_field_flexible)
FIELD_OFFSETS(OFFSET)

/* offsetof of a member of a record of the C library by its type name */
#define LIBRARY_OFFSET(N, T, M)        \
        long library_##N(void)         \
        {                              \
                return offsetof(T, M); \
        }

LIBRARY_OFFSETS(LIBRARY_OFFSET)

/* the place of a named bit-field by the static data alone: the field
   given all ones, -1 converted, in a static union of the record and
   its bytes, and the bytes read back, as the lowest bit set times 100
   plus the count of bits set; no offsetof and no access of the field */
#define BITS(K, N, F)                                            \
        long bits_##N##_##F(void)                                \
        {                                                        \
                static union                                     \
                {                                                \
                        K N s;                                   \
                        unsigned char b[sizeof(K N)];            \
                } u = { .s.F = -1 };                             \
                long low = -1, count = 0;                        \
                                                                 \
                for (long i = 0; i < 8 * (long) sizeof u.b; ++i) \
                        if ((u.b[i / 8] >> i % 8) & 1) {         \
                                if (low < 0)                     \
                                        low = i;                 \
                                ++count;                         \
                        }                                        \
                                                                 \
                return low * 100 + count;                        \
        }

#define FIELD_PAIR_BITS(F, TF, WF, K, TK) \
        BITS(struct, fk_##F##_##K, f)     \
        BITS(struct, kf_##F##_##K, f)

#define FIELD_PAIRS_BITS(F, TF, WF) FIELD_KINDS(FIELD_PAIR_BITS, F, TF, WF)

FIELDS(FIELD_PAIRS_BITS)

#define FIELD_TWO_BITS(A, TA, WA, B, TB, WB) \
        BITS(struct, ff_##A##_##B, x)        \
        BITS(struct, ff_##A##_##B, y)

#define FIELD_TWOS_BITS(A, TA, WA) FEW_FIELDS2(FIELD_TWO_BITS, A, TA, WA)

FEW_FIELDS(FIELD_TWOS_BITS)

FIELD_BITS(BITS)
