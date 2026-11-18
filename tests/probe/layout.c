/* The layout probe: the size, the alignment and the member offsets of
   structs and unions, in rcc's C, docs/shape.md chapter 3, the stride
   of an array of each, which c1 computes from the size, and the place
   of an object of each.  See layout.h. */

#include "layout.h"

/* the functions every record has: its size, its alignment, the size of
   an array of three, the distance from an element's address to the
   next one's, and whether a local and a global of it, each after a char
   that would leave it at an odd address, are aligned: 0 when both are */
#define RECORD(K, N)                                                    \
    long size_##N(void) { return sizeof(K N); }                         \
    long align_##N(void) { return _Alignof(K N); }                      \
    long array_##N(void) { return sizeof(K N[3]); }                     \
    long stride_##N(K N *p) { return (char *) &p[1] - (char *) p; }     \
    char before_##N;                                                    \
    K N global_##N;                                                     \
    long place_##N(void)                                                \
    {                                                                   \
        char c;                                                         \
        K N local;                                                      \
                                                                        \
        return ((unsigned long) &local % _Alignof(K N) != 0) * 2 +      \
               ((unsigned long) &global_##N % _Alignof(K N) != 0);      \
    }

/* the offset of a member */
#define OFFSET(K, N, M)                                                 \
    long offset_##N##_##M(void) { return offsetof(K N, M); }

#define PAIR_FUNCTIONS(A, TA, B, TB)                                    \
    RECORD(struct, p_##A##B)                                            \
    OFFSET(struct, p_##A##B, x)                                         \
    OFFSET(struct, p_##A##B, y)                                         \
    RECORD(union, u_##A##B)                                             \
    OFFSET(union, u_##A##B, x)                                          \
    OFFSET(union, u_##A##B, y)

#define PAIRS_FUNCTIONS(A, TA) KINDS2(PAIR_FUNCTIONS, A, TA)

KINDS(PAIRS_FUNCTIONS)

#define TRIPLE_FUNCTIONS(A, TA, B, TB, C, TC)                           \
    RECORD(struct, t_##A##B##C)                                         \
    OFFSET(struct, t_##A##B##C, x)                                      \
    OFFSET(struct, t_##A##B##C, y)                                      \
    OFFSET(struct, t_##A##B##C, z)

#define TRIPLES_OF_FUNCTIONS(A, TA, B, TB)                              \
    FEW3(TRIPLE_FUNCTIONS, A, TA, B, TB)
#define TRIPLES_FUNCTIONS(A, TA) FEW2(TRIPLES_OF_FUNCTIONS, A, TA)

FEW(TRIPLES_FUNCTIONS)

RECORDS(RECORD)

/* offsetof by a designator that goes into the members' records */
#define DESIGNATED(N, R, D)                                             \
    long designated_##N(void) { return offsetof(R, D); }

DESIGNATORS(DESIGNATED)

/* an enumeration constant the member list of struct tagged declares */
int
mode_two(void)
{
        return M2;
}
