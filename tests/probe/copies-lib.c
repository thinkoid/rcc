/* The copies probe's library, compiled by clang alone.  clang copies a
   large record by a call of memcpy, which the renaming of the
   reference makes a call of ref_memcpy: the reference's copies call
   the C library's memcpy through it.  rcc's copies call nothing. */

#include <string.h>

void *
ref_memcpy(void *to, const void *from, size_t size)
{
        return memcpy(to, from, size);
}
