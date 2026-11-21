/* The generic probe's main, compiled by clang -O0: calls each function
   of generic.c as rcc compiled it and as clang did (ref_), and compares
   the numbers they select.  Prints each difference and a summary; exits
   0 when nothing differs, 1 otherwise. */

#include <stdio.h>

#include "generic.h"

static long total;
static long bad;

static void
report(const char *name, int got, int want)
{
        ++total;

        if (got == want)
                return;

        if (++bad <= 40)
                printf("%s: rcc %d, clang %d\n", name, got, want);
}

/* each function of generic.c, as rcc compiled it and as clang did */
#define DECLARE(N, E, ...) int select_##N(void), ref_select_##N(void);

SELECTIONS(DECLARE)

#define CHECK(N, E, ...) report(#N, select_##N(), ref_select_##N());

int
main(void)
{
        SELECTIONS(CHECK)

        printf("%ld checks, %ld differ\n", total, bad);

        return bad != 0;
}
