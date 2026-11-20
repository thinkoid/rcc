/* The variadics probe's main, compiled by clang -O0: calls every
   function of variadics.c as rcc compiled it and as clang did (ref_)
   with the same arguments, and compares the results: for each type,
   each count of named parameters and each count of arguments, from
   none to MOST; the types of a pattern after each count of named
   parameters; the text vsnprintf makes of a list rcc's va_start built,
   with doubles among the arguments, which the caller passes in vector
   registers; a copied list, one started again, one started through a
   pointer stepped once, and one handed to clang's code; and a variadic
   call rcc's code makes.  Prints each difference and a summary; exits
   0 when nothing differs, 1 otherwise. */

#include <stdio.h>
#include <string.h>

#include "variadics.h"

static char text[MOST];

/* MOST arguments of each type, as the lists probe passes them */
#define VALUES_i                                                             \
        -1, 2, 0x7fffffff, -0x7fffffff - 1, 5, -6, 7, 8, 9, 10, 11, -12, 13, \
            14, 15, 16, 17, 18, 19, -20
#define VALUES_u                                                             \
        0xffffffffu, 2u, 0x80000000u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, \
            13u, 14u, 15u, 16u, 17u, 18u, 19u, 0xfffffffeu
#define VALUES_l                                                            \
        -1L, 0x123456789abcdefL, -0x7fffffffffffffffL - 1, 4L, 5L, -6L, 7L, \
            8L, 9L, 10L, 11L, 12L, 13L, 14L, 15L, 16L, 17L, 18L, 19L,       \
            0x7fffffffffffffffL
#define VALUES_m                                                            \
        0xfffffffffffffffful, 2ul, 3ul, 4ul, 5ul, 6ul, 7ul, 8ul, 9ul, 10ul, \
            11ul, 12ul, 13ul, 14ul, 15ul, 16ul, 17ul, 18ul, 19ul,           \
            0x8000000000000000ul
#define VALUES_p                                                              \
        text, text + 1, text + 2, text + 3, text + 4, text + 5, text + 6,     \
            text + 7, text + 8, text + 9, text + 10, text + 11, text + 12,    \
            text + 13, text + 14, text + 15, text + 16, text + 17, text + 18, \
            text + 19
#define VALUES_c                                                           \
        (char) -3, 'a', (char) 0x80, (char) 0x7f, 'e', 'f', 'g', 'h', 'i', \
            'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', (char) 0xff
#define VALUES_s                                                              \
        (short) -30000, 2, (short) 0x8000, 0x7fff, 5, 6, 7, 8, 9, 10, 11, 12, \
            13, 14, 15, 16, 17, 18, 19, (short) 0xffff

/* each function of variadics.c, and its ref_ copy */
#define DECLARE(K, A)                          \
        long sum##K##_##A(NAMED_##K int, ...); \
        long ref_sum##K##_##A(NAMED_##K int, ...);

#define DECLARES(A, T) COUNTS(DECLARE, A)

TYPES(DECLARES)

#define DECLARE_MIXED(K, A)                         \
        long mixed##K(NAMED_##K const char *, ...); \
        long ref_mixed##K(NAMED_##K const char *, ...);

COUNTS(DECLARE_MIXED, )

int format(char *, size_t, const char *, ...);
int ref_format(char *, size_t, const char *, ...);
int format_through(char *, size_t, const char *, ...);
int ref_format_through(char *, size_t, const char *, ...);
long copied(int, ...), ref_copied(int, ...);
long restarted(int, ...), ref_restarted(int, ...);
long relayed(int, ...), ref_relayed(int, ...);
long handed(int, ...), ref_handed(int, ...);
long started(int, ...), ref_started(int, ...);
long called(void), ref_called(void);

static long total;
static long bad;

static void
report(const char *name, int k, int count, long got, long want)
{
        ++total;

        if (got == want)
                return;

        if (++bad <= 40)
                printf("%s, %d named, %d read: rcc %ld, clang %ld\n", name, k,
                       count, got, want);
}

/* count arguments of A after k named parameters, for each count */
#define CHECK(K, A)                                                    \
        for (int count = 0; count <= MOST; ++count)                    \
                report("sum" #K "_" #A, K, count,                      \
                       sum##K##_##A(NAMED_ARGS_##K count, VALUES_##A), \
                       ref_sum##K##_##A(NAMED_ARGS_##K count, VALUES_##A));

#define CHECKS(A, T) COUNTS(CHECK, A)

/* the types once each, twice, after k named parameters */
#define PATTERN                                                          \
        -1, 0xffffffffu, -2L, 0xfffffffffffffffful, text + 3, (char) -3, \
            (short) -4
#define CHECK_MIXED(K, A)                                                \
        report(                                                          \
            "mixed" #K, K, 14,                                           \
            mixed##K(NAMED_ARGS_##K "iulmpcsiulmpcs", PATTERN, PATTERN), \
            ref_mixed##K(NAMED_ARGS_##K "iulmpcsiulmpcs", PATTERN, PATTERN));

/* a format of every kind of argument, doubles among them, and the
   text each build's list makes of it */
#define FORM "%d %u %ld %lu %s %c %f %e %g %d %ld %.3f %s %d %f %ld %g"
#define FORM_ARGS                                                             \
        -1, 2u, -3L, 4ul, "five", 'x', 1.5, -2.25e10, 0.125, 9, 10L, 3.14159, \
            "thirteen", 14, -15.5, 16L, 17e-3

static void
check_format(void)
{
        char got[256], want[256];

        report("format", 3, 17, format(got, sizeof got, FORM, FORM_ARGS),
               ref_format(want, sizeof want, FORM, FORM_ARGS));
        report("format text", 3, 17, strcmp(got, want), 0);

        report("format_through", 3, 17,
               format_through(got, sizeof got, FORM, FORM_ARGS),
               ref_format_through(want, sizeof want, FORM, FORM_ARGS));
        report("format_through text", 3, 17, strcmp(got, want), 0);
}

int
main(void)
{
        TYPES(CHECKS)
        COUNTS(CHECK_MIXED, )
        check_format();

        for (int count = 0; count <= MOST; ++count) {
                report("copied", 0, count, copied(count, VALUES_i),
                       ref_copied(count, VALUES_i));
                report("restarted", 0, count, restarted(count, VALUES_i),
                       ref_restarted(count, VALUES_i));
                report("relayed", 0, count, relayed(count, VALUES_l),
                       ref_relayed(count, VALUES_l));
                report("started", 0, count, started(count, VALUES_i),
                       ref_started(count, VALUES_i));
        }

        for (int count = 0; count <= MOST / 2; ++count)
                report("handed", 0, count, handed(count, VALUES_l),
                       ref_handed(count, VALUES_l));

        report("called", 0, 0, called(), ref_called());

        printf("%ld checks, %ld differ\n", total, bad);

        return bad != 0;
}
