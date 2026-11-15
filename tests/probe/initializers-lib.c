/* The initializers probe's library, compiled by clang alone and linked
   twice, once as it is for rcc's build and once renamed under the
   prefix ref_ for the reference's, so each build has its own copy.  It
   defines what initializers.c only declares, the array without its
   size there, and sums the array initializers.c defines, which it
   declares without its size.  Its own objects are initialized by
   clang with an address constant or a string, and initializers.c
   reads them.  See initializers.h. */

#include "initializers.h"

int lib_object = 31;
int lib_array[ARRAY] = { 10, 11, 12, 13, 14, 15, 16, 17 };

int
lib_triple(int v)
{
        return 3 * v;
}

long
lib_total(void)
{
        long sum = 0;

        for (int i = 0; i < PROBE_ARRAY; ++i)
                sum += probe_array[i] * (long) (i + 1);
        return sum;
}

int *lib_element = &lib_array[6];
char *lib_suffix = WORD + 2;
char lib_word[] = WORD;
int (*lib_function)(int) = lib_triple;
