/* The functions memory.c calls, compiled by clang -O0 alone.  The
   object is linked twice, as it is for rcc's build of memory.c and
   under the prefix ref_ for clang's.  Each reads and writes only
   through the pointers it is passed. */

#include "memory.h"

/* The low four bits of the frame address, which are zero when the
   caller kept the stack pointer a multiple of 16 at the call.  Added
   to every result, so a caller that did not shows as a difference. */
#define SKEW                                                            \
    ((long) ((unsigned long) __builtin_frame_address(0) & 15) * 1000003L)

/* the object read and written; n objects written, and summed; n rows
   of five written, and summed */
#define LIBRARY(T, N)                                                   \
    long touch_##N(T *p)                                                \
    {                                                                   \
        const T old = *p;                                               \
                                                                        \
        *p = (T) ((unsigned long) old * 3 + 1);                         \
                                                                        \
        return (long) old + SKEW;                                       \
    }                                                                   \
                                                                        \
    long fill_##N(T *a, int n, long v)                                  \
    {                                                                   \
        for (int k = 0; k < n; ++k)                                     \
            a[k] = (T) ((unsigned long) v + 7UL * k);                   \
                                                                        \
        return SKEW;                                                    \
    }                                                                   \
                                                                        \
    long total_##N(T *a, int n)                                         \
    {                                                                   \
        unsigned long sum = 0;                                          \
                                                                        \
        for (int k = 0; k < n; ++k)                                     \
            sum = sum * 3 + (unsigned long) (long) a[k];                \
                                                                        \
        return (long) sum + SKEW;                                       \
    }                                                                   \
                                                                        \
    long fill_rows_##N(T m[][5], int n, long v)                         \
    {                                                                   \
        for (int i = 0; i < n; ++i)                                     \
            for (int j = 0; j < 5; ++j)                                 \
                m[i][j] = (T) ((unsigned long) v * (i + 1) + j);        \
                                                                        \
        return SKEW;                                                    \
    }                                                                   \
                                                                        \
    long total_rows_##N(T m[][5], int n)                                \
    {                                                                   \
        unsigned long sum = 0;                                          \
                                                                        \
        for (int i = 0; i < n; ++i)                                     \
            for (int j = 0; j < 5; ++j)                                 \
                sum = sum * 5 + (unsigned long) (long) m[i][j];         \
                                                                        \
        return (long) sum + SKEW;                                       \
    }

TYPES(LIBRARY)

/* the bytes of a string, to its terminator */
long text(const char *s)
{
    unsigned long sum = 0;

    for (; *s; ++s)
        sum = sum * 31 + (unsigned char) *s;

    return (long) sum + SKEW;
}

long mix1(long a) { return a * 3 + SKEW; }
long mix2(long a, long b) { return a * 3 + b * 5 + SKEW; }
