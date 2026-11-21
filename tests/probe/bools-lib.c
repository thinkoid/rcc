/* The bools probe's library, compiled by clang alone: rcc's code calls
   it, passing a _Bool and reading one returned, so that the convention
   is proved from rcc's side.  clang zeroes a local record a list
   initializes by a call of memset on x86-64, which the renaming of the
   reference makes a call of ref_memset: the reference's zeroing calls
   the C library's memset through it.  rcc's zeroing calls nothing. */

#include <string.h>

#include "bools.h"

_Bool
lib_negate(int x)
{
        return !x;
}

_Bool
lib_identity(_Bool b)
{
        return b;
}

void *
ref_memset(void *to, int c, size_t size)
{
        return memset(to, c, size);
}
