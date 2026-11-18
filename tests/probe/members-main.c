/* The members probe's main, compiled by clang -O0: calls every function
   of members.c as rcc compiled it and as clang did (ref_), on records
   filled alike, and compares the results and the records' bytes after
   each write.  Prints each difference and a summary; exits 0 when
   nothing differs, 1 otherwise. */

#include <stdio.h>
#include <string.h>

#include "members.h"

static const long values[] = { 0,      1,           -1,
                               0x7f,   0x80,        0xff,
                               0x1234, -0x5678,     0x12345678,
                               -2,     0x123456789abcdef0L };

#define COUNT ((int) (sizeof values / sizeof *values))

static long total;
static long bad;

static void
report(const char *name, long v, long got, long want)
{
        ++total;

        if (got == want)
                return;

        if (++bad <= 40)
                printf("%s(%ld): rcc %ld, clang %ld\n", name, v, got, want);
}

/* the bytes of two records written alike, compared */
static void
same(const char *name, long v, const void *a, const void *b, size_t size)
{
        report(name, v, memcmp(a, b, size) != 0, 0);
}

/* a record's bytes, each its own value */
static void
fill(void *p, size_t size)
{
        unsigned char *bytes = p;

        for (size_t i = 0; i < size; ++i)
                bytes[i] = (unsigned char) (i * 29 + 7);
}

/* each function of members.c, as rcc compiled it and as clang did */
#define MEMBER(K, N, M, A)                                              \
    long get_##N##_##M(K N *), ref_get_##N##_##M(K N *);                \
    void set_##N##_##M(K N *, long), ref_set_##N##_##M(K N *, long);    \
    long step_##N##_##M(K N *), ref_step_##N##_##M(K N *);              \
    long add_##N##_##M(K N *), ref_add_##N##_##M(K N *);                \
    long where_##N##_##M(K N *), ref_where_##N##_##M(K N *);            \
    long second_##N##_##M(K N *), ref_second_##N##_##M(K N *);          \
    void held_##N##_##M(K N *, int, long);                              \
    void ref_held_##N##_##M(K N *, int, long);                          \
    long held_add_##N##_##M(K N *, int);                                \
    long ref_held_add_##N##_##M(K N *, int);                            \
    long local_##N##_##M(long), ref_local_##N##_##M(long);              \
    long global_##N##_##M(long), ref_global_##N##_##M(long);

#define PAIR_MEMBERS(A, TA, B, TB)                                      \
    MEMBER(struct, p_##A##B, x, A)                                      \
    MEMBER(struct, p_##A##B, y, B)                                      \
    MEMBER(union, u_##A##B, x, A)                                       \
    MEMBER(union, u_##A##B, y, B)

#define PAIRS_MEMBERS(A, TA) KINDS2(PAIR_MEMBERS, A, TA)

KINDS(PAIRS_MEMBERS)

#define TRIPLE_MEMBERS(A, TA, B, TB, C, TC)                             \
    MEMBER(struct, t_##A##B##C, x, A)                                   \
    MEMBER(struct, t_##A##B##C, y, B)                                   \
    MEMBER(struct, t_##A##B##C, z, C)

#define TRIPLES_OF_MEMBERS(A, TA, B, TB) FEW3(TRIPLE_MEMBERS, A, TA, B, TB)
#define TRIPLES_MEMBERS(A, TA) FEW2(TRIPLES_OF_MEMBERS, A, TA)

FEW(TRIPLES_MEMBERS)

long walk(struct link *), ref_walk(struct link *);
long walk_pairs(struct link *), ref_walk_pairs(struct link *);
long walk_count(struct link *), ref_walk_count(struct link *);
int apply(struct ops *, int), ref_apply(struct ops *, int);

/* a member's checks for each value: written, read, stepped and added
   to on two records filled alike, its offset, the second element's
   read, written and added to across a call, and a local's and a
   global's write and read */
#define CHECK_MEMBER(K, N, M, A)                                        \
    for (int k = 0; k < COUNT; ++k) {                                   \
        K N a[2], b[2];                                                 \
        const long v = values[k];                                       \
                                                                        \
        fill(a, sizeof a);                                              \
        fill(b, sizeof b);                                              \
        set_##N##_##M(a, v);                                            \
        ref_set_##N##_##M(b, v);                                        \
        same("set_" #N "_" #M, v, a, b, sizeof a);                      \
        report("get_" #N "_" #M, v, get_##N##_##M(a),                   \
               ref_get_##N##_##M(b));                                   \
        report("step_" #N "_" #M, v, step_##N##_##M(a),                 \
               ref_step_##N##_##M(b));                                  \
        same("step_" #N "_" #M, v, a, b, sizeof a);                     \
        report("add_" #N "_" #M, v, add_##N##_##M(a),                   \
               ref_add_##N##_##M(b));                                   \
        same("add_" #N "_" #M, v, a, b, sizeof a);                      \
        report("where_" #N "_" #M, v, where_##N##_##M(a),               \
               ref_where_##N##_##M(b));                                 \
        report("second_" #N "_" #M, v, second_##N##_##M(a),             \
               ref_second_##N##_##M(b));                                \
        held_##N##_##M(a, 1, v);                                        \
        ref_held_##N##_##M(b, 1, v);                                    \
        same("held_" #N "_" #M, v, a, b, sizeof a);                     \
        report("held_add_" #N "_" #M, v, held_add_##N##_##M(a, 1),      \
               ref_held_add_##N##_##M(b, 1));                           \
        same("held_add_" #N "_" #M, v, a, b, sizeof a);                 \
        report("local_" #N "_" #M, v, local_##N##_##M(v),               \
               ref_local_##N##_##M(v));                                 \
        report("global_" #N "_" #M, v, global_##N##_##M(v),             \
               ref_global_##N##_##M(v));                                \
    }

#define CHECK_PAIR(A, TA, B, TB)                                        \
    CHECK_MEMBER(struct, p_##A##B, x, A)                                \
    CHECK_MEMBER(struct, p_##A##B, y, B)                                \
    CHECK_MEMBER(union, u_##A##B, x, A)                                 \
    CHECK_MEMBER(union, u_##A##B, y, B)

#define CHECK_PAIRS(A, TA) KINDS2(CHECK_PAIR, A, TA)

#define CHECK_TRIPLE(A, TA, B, TB, C, TC)                               \
    CHECK_MEMBER(struct, t_##A##B##C, x, A)                             \
    CHECK_MEMBER(struct, t_##A##B##C, y, B)                             \
    CHECK_MEMBER(struct, t_##A##B##C, z, C)

#define CHECK_TRIPLES_OF(A, TA, B, TB) FEW3(CHECK_TRIPLE, A, TA, B, TB)
#define CHECK_TRIPLES(A, TA) FEW2(CHECK_TRIPLES_OF, A, TA)

static int
twice(int x)
{
        return 2 * x;
}

/* the walks over lists of each length up to eight, and the call
   through a member */
static void
test_links(void)
{
        struct link links[8];

        for (int n = 0; n <= 8; ++n) {
                for (int i = 0; i < n; ++i) {
                        links[i].value = values[i % COUNT] + i;
                        links[i].next = i + 1 < n ? &links[i + 1] : 0;
                }

                struct link *list = n ? links : 0;

                report("walk", n, walk(list), ref_walk(list));
                report("walk_pairs", n, walk_pairs(list), ref_walk_pairs(list));
                report("walk_count", n, walk_count(list), ref_walk_count(list));
        }

        struct ops o = { 5, twice };

        for (int k = 0; k < COUNT; ++k)
                report("apply", values[k], apply(&o, (int) values[k]),
                       ref_apply(&o, (int) values[k]));
}

int
main(void)
{
        KINDS(CHECK_PAIRS)
        FEW(CHECK_TRIPLES)
        test_links();

        printf("%ld checks, %ld differ\n", total, bad);

        return bad != 0;
}
