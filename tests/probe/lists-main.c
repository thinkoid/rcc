/* The lists probe's main, compiled by clang -O0: calls every function
   of lists-lib.c, whose lists reach rcc's build of lists.c, and the
   same under the prefix ref_, whose lists reach clang's, with the same
   arguments, and compares the results: for each type, each count of
   named parameters before the list and each count of arguments read,
   from none to MOST; the types of a pattern, each after every count of
   named parameters; and a list read through a copy.  Prints each difference and a summary;
   exits 0 when nothing differs, 1 otherwise. */

#include <stdio.h>

#include "lists.h"

static char text[MOST];

/* MOST arguments of each type, past which a call reads none */
#define VALUES_i                                                        \
    -1, 2, 0x7fffffff, -0x7fffffff - 1, 5, -6, 7, 8, 9, 10, 11, -12,    \
        13, 14, 15, 16, 17, 18, 19, -20
#define VALUES_u                                                        \
    0xffffffffu, 2u, 0x80000000u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u,     \
        12u, 13u, 14u, 15u, 16u, 17u, 18u, 19u, 0xfffffffeu
#define VALUES_l                                                        \
    -1L, 0x123456789abcdefL, -0x7fffffffffffffffL - 1, 4L, 5L, -6L, 7L, \
        8L, 9L, 10L, 11L, 12L, 13L, 14L, 15L, 16L, 17L, 18L, 19L,       \
        0x7fffffffffffffffL
#define VALUES_m                                                        \
    0xfffffffffffffffful, 2ul, 3ul, 4ul, 5ul, 6ul, 7ul, 8ul, 9ul, 10ul, \
        11ul, 12ul, 13ul, 14ul, 15ul, 16ul, 17ul, 18ul, 19ul,           \
        0x8000000000000000ul
#define VALUES_p                                                        \
    text, text + 1, text + 2, text + 3, text + 4, text + 5, text + 6,   \
        text + 7, text + 8, text + 9, text + 10, text + 11, text + 12,  \
        text + 13, text + 14, text + 15, text + 16, text + 17,          \
        text + 18, text + 19
#define VALUES_c                                                        \
    (char) -3, 'a', (char) 0x80, (char) 0x7f, 'e', 'f', 'g', 'h', 'i',  \
        'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', (char) 0xff
#define VALUES_s                                                        \
    (short) -30000, 2, (short) 0x8000, 0x7fff, 5, 6, 7, 8, 9, 10, 11,   \
        12, 13, 14, 15, 16, 17, 18, 19, (short) 0xffff

/* each function of lists-lib.c, and its ref_ copy */
#define DECLARE(K, A)                                                   \
    long wrap##K##_##A(NAMED_##K int, ...);                             \
    long ref_wrap##K##_##A(NAMED_##K int, ...);

#define DECLARES(A, T) COUNTS(DECLARE, A)

TYPES(DECLARES)

#define DECLARE_MIXED(K, A)                                             \
    long wrap##K##_mixed(NAMED_##K const char *, ...);                  \
    long ref_wrap##K##_mixed(NAMED_##K const char *, ...);

COUNTS(DECLARE_MIXED, )

long wrap_copied(int, ...), ref_wrap_copied(int, ...);

static long total;
static long bad;

static void
report(const char *name, int k, int count, long got, long want)
{
        ++total;

        if (got == want)
                return;

        if (++bad <= 40)
                printf("%s, %d named, %d read: rcc %ld, clang %ld\n", name,
                       k, count, got, want);
}

/* count arguments of A after k named parameters, for each count */
#define CHECK(K, A)                                                     \
    for (int count = 0; count <= MOST; ++count)                         \
        report("wrap" #K "_" #A, K, count,                              \
               wrap##K##_##A(NAMED_ARGS_##K count, VALUES_##A),         \
               ref_wrap##K##_##A(NAMED_ARGS_##K count, VALUES_##A));

#define CHECKS(A, T) COUNTS(CHECK, A)

/* the types once each, twice, read after k named parameters, so that
   each type stands at every position from the first register on */
#define PATTERN                                                         \
    -1, 0xffffffffu, -2L, 0xfffffffffffffffful, text + 3, (char) -3,    \
        (short) -4
#define CHECK_MIXED(K, A)                                               \
    report("wrap" #K "_mixed", K, 14,                                   \
           wrap##K##_mixed(NAMED_ARGS_##K "iulmpcsiulmpcs", PATTERN,    \
                           PATTERN),                                    \
           ref_wrap##K##_mixed(NAMED_ARGS_##K "iulmpcsiulmpcs",         \
                               PATTERN, PATTERN));

int
main(void)
{
        TYPES(CHECKS)
        COUNTS(CHECK_MIXED, )

        for (int count = 0; count <= MOST; ++count)
                report("wrap_copied", 0, count,
                       wrap_copied(count, VALUES_i),
                       ref_wrap_copied(count, VALUES_i));

        printf("%ld checks, %ld differ\n", total, bad);

        return bad != 0;
}
