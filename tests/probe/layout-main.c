/* The layout probe's main, compiled by clang -O0: calls every function
   of layout.c as rcc compiled it and as clang did (ref_), and compares.
   Prints each difference and a summary; exits 0 when nothing differs,
   1 otherwise.  layout.h comes first, since it asks for GNU's
   declarations before any header is read. */

#include "layout.h"

#include <stdio.h>

static long total;
static long bad;

static void
report(const char *name, long got, long want)
{
        ++total;

        if (got == want)
                return;

        if (++bad <= 40)
                printf("%s: rcc %ld, clang %ld\n", name, got, want);
}

/* each function of layout.c, as rcc compiled it and as clang did */
#define RECORD(K, N)                                   \
        long size_##N(void), ref_size_##N(void);       \
        long align_##N(void), ref_align_##N(void);     \
        long array_##N(void), ref_array_##N(void);     \
        long stride_##N(K N *), ref_stride_##N(K N *); \
        long place_##N(void), ref_place_##N(void);

#define OFFSET(K, N, M) long offset_##N##_##M(void), ref_offset_##N##_##M(void);

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

#define FLEXIBLE_RECORD(K, N)                      \
        long size_##N(void), ref_size_##N(void);   \
        long align_##N(void), ref_align_##N(void); \
        long place_##N(void), ref_place_##N(void);

FLEXIBLE(FLEXIBLE_RECORD)

#define DESIGNATED(N, R, D) long designated_##N(void), ref_designated_##N(void);

DESIGNATORS(DESIGNATED)

#define LIST_OFFSET(M) long list_##M(void), ref_list_##M(void);

LIST_MEMBERS(LIST_OFFSET)

int mode_two(void), ref_mode_two(void);

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

#define LIBRARY_OFFSET(N, T, M) long library_##N(void), ref_library_##N(void);

LIBRARY_OFFSETS(LIBRARY_OFFSET)

/* a record's checks; the stride is of two elements of the record */
#define CHECK_RECORD(K, N)                                                  \
        {                                                                   \
                K N two[2];                                                 \
                                                                            \
                report("size_" #N, size_##N(), ref_size_##N());             \
                report("align_" #N, align_##N(), ref_align_##N());          \
                report("array_" #N, array_##N(), ref_array_##N());          \
                report("stride_" #N, stride_##N(two), ref_stride_##N(two)); \
                report("place_" #N, place_##N(), ref_place_##N());          \
        }

/* a record's checks when it holds a flexible array member, which is no
   element of an array */
#define CHECK_FLEXIBLE(K, N)                                       \
        {                                                          \
                report("size_" #N, size_##N(), ref_size_##N());    \
                report("align_" #N, align_##N(), ref_align_##N()); \
                report("place_" #N, place_##N(), ref_place_##N()); \
        }

#define CHECK_OFFSET(K, N, M) \
        report("offset_" #N "_" #M, offset_##N##_##M(), ref_offset_##N##_##M());

#define CHECK_PAIR(A, TA, B, TB)          \
        CHECK_RECORD(struct, p_##A##B)    \
        CHECK_OFFSET(struct, p_##A##B, x) \
        CHECK_OFFSET(struct, p_##A##B, y) \
        CHECK_RECORD(union, u_##A##B)     \
        CHECK_OFFSET(union, u_##A##B, x)  \
        CHECK_OFFSET(union, u_##A##B, y)

#define CHECK_PAIRS(A, TA) KINDS2(CHECK_PAIR, A, TA)

#define CHECK_TRIPLE(A, TA, B, TB, C, TC)    \
        CHECK_RECORD(struct, t_##A##B##C)    \
        CHECK_OFFSET(struct, t_##A##B##C, x) \
        CHECK_OFFSET(struct, t_##A##B##C, y) \
        CHECK_OFFSET(struct, t_##A##B##C, z)

#define CHECK_TRIPLES_OF(A, TA, B, TB) FEW3(CHECK_TRIPLE, A, TA, B, TB)
#define CHECK_TRIPLES(A, TA)           FEW2(CHECK_TRIPLES_OF, A, TA)

#define CHECK_FLOATING_PAIRS(A, TA) FLOATING2(CHECK_PAIR, A, TA)

#define CHECK_FLOATING_TRIPLES_OF(A, TA, B, TB) FEW3(CHECK_TRIPLE, A, TA, B, TB)
#define CHECK_FLOATING_TRIPLES(A, TA) \
        FLOATING2(CHECK_FLOATING_TRIPLES_OF, A, TA)

#define CHECK_DESIGNATED(N, R, D) \
        report("designated_" #N, designated_##N(), ref_designated_##N());

#define CHECK_LIST(M) report("list_" #M, list_##M(), ref_list_##M());

#define CHECK_FIELD_PAIR(F, TF, WF, K, TK)    \
        CHECK_RECORD(struct, fk_##F##_##K)    \
        CHECK_OFFSET(struct, fk_##F##_##K, y) \
        CHECK_RECORD(struct, kf_##F##_##K)    \
        CHECK_OFFSET(struct, kf_##F##_##K, x)

#define CHECK_FIELD_PAIRS(F, TF, WF) FIELD_KINDS(CHECK_FIELD_PAIR, F, TF, WF)

#define CHECK_FIELD_TWO(A, TA, WA, B, TB, WB) \
        CHECK_RECORD(struct, ff_##A##_##B)    \
        CHECK_OFFSET(struct, ff_##A##_##B, z)

#define CHECK_FIELD_TWOS(A, TA, WA) FEW_FIELDS2(CHECK_FIELD_TWO, A, TA, WA)

#define CHECK_LIBRARY(N, T, M) \
        report("library_" #N, library_##N(), ref_library_##N());

int
main(void)
{
        KINDS(CHECK_PAIRS)
        FEW(CHECK_TRIPLES)
        KINDS(CHECK_FLOATING_PAIRS)
        FLOATING(CHECK_PAIRS)
        FLOATING(CHECK_FLOATING_PAIRS)
        FEW(CHECK_FLOATING_TRIPLES)
        RECORDS(CHECK_RECORD)
        FLEXIBLE(CHECK_FLEXIBLE)
        DESIGNATORS(CHECK_DESIGNATED)
        LIST_MEMBERS(CHECK_LIST)
        report("mode_two", mode_two(), ref_mode_two());
        FIELDS(CHECK_FIELD_PAIRS)
        FEW_FIELDS(CHECK_FIELD_TWOS)
        FIELD_RECORDS(CHECK_RECORD)
        CHECK_FLEXIBLE(struct, anonymous_field_flexible)
        FIELD_OFFSETS(CHECK_OFFSET)
        LIBRARY_OFFSETS(CHECK_LIBRARY)

        printf("%ld checks, %ld differ\n", total, bad);

        return bad != 0;
}
