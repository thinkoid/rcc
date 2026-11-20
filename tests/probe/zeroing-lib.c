/* The zeroing probe's library, compiled by clang alone.  taken() keeps
   a local's bytes at to for the main, then fills the local with 0xa5,
   so that the declaration reached again finds no zero byte in its
   slot.  clang zeroes a large local by
   a call of memset, which the renaming of the reference makes a call
   of ref_memset: the reference's calls reach the C library's memset
   through it.  rcc's zeroing calls nothing. */

#include <string.h>

void
taken(void *to, void *from, unsigned long size)
{
        unsigned char *kept = to;
        unsigned char *local = from;

        for (unsigned long i = 0; i < size; ++i) {
                kept[i] = local[i];
                local[i] = 0xa5;
        }
}

void *
ref_memset(void *to, int c, size_t size)
{
        return memset(to, c, size);
}
