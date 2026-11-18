/* The variadics probe's library, compiled by clang alone: a function
   that reads a list rcc's va_start built, and the reference's name of
   vsnprintf.  The object is linked twice, as it is for rcc's build of
   variadics.c and under the prefix ref_ for clang's.  The renaming of
   the reference makes its call of vsnprintf a call of ref_vsnprintf,
   which calls the C library's here.  See variadics.h. */

#include <stdio.h>

#include "variadics.h"

long
list_sum(int count, va_list ap)
{
        long h = 41;

        while (count-- > 0)
                h = h * 1000003 + va_arg(ap, long);

        return h;
}

int
ref_vsnprintf(char *buffer, size_t size, const char *form, va_list ap)
{
        return vsnprintf(buffer, size, form, ap);
}
