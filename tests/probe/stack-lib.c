/* The functions stack.c calls, compiled by clang -O0 alone.  The
   object is linked twice, as it is for rcc's build of stack.c and
   under the prefix ref_ for clang's. */

#include "stack.h"

/* the elements, each read by clang's code, in order */
long
sum(const long *v, long n)
{
        long s = 0;

        for (long i = 0; i < n; i++)
                s = s * 31 + v[i];

        return s;
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

/* the frame's address modulo 16: clang's frame record lies 16 bytes
   below the caller's stack pointer at the call on either target, so it
   is 0 where that stack pointer was a multiple of 16 */
long
frame_misalignment(void)
{
        return (long) ((unsigned long) __builtin_frame_address(0) & 15);
}

/* a frame of 512 bytes written whole, below the caller's stack
   pointer */
long
scribble(long v)
{
        volatile long pad[64];

        for (int i = 0; i < 64; i++)
                pad[i] = v + i;

        return pad[63];
}
