/* The functions blocks.c calls, compiled by clang -O0 alone.  The
   object is linked twice, as it is for rcc's build of blocks.c and
   under the prefix ref_ for clang's, so each build keeps a record of
   its own. */

#include "blocks.h"

/* the calls made since the main cleared it, each one's argument in
   the order made */
long trace;

long
mark(long n)
{
        trace = trace * 31 + n;

        return n;
}

/* each argument in its own place, so one passed in the wrong place or
   in the wrong order shows */
long
ten(long a,
    long b,
    long c,
    long d,
    long e,
    long f,
    long g,
    long h,
    long i,
    long j)
{
        return a + 3 * b + 5 * c + 7 * d + 11 * e + 13 * f + 17 * g + 19 * h +
               23 * i + 29 * j;
}
