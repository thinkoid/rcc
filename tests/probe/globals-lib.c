/* The globals probe's library, compiled by clang alone and linked
   twice, once as it is for rcc's build and once renamed under the
   prefix ref_ for the reference's: so each build has its own copy of
   the objects defined here, as of those globals.c defines.  It defines
   the objects globals.c only declares, which rcc's build reaches
   through the GOT, and the functions globals.c calls.  See globals.h. */

#include "globals.h"

#define LIBRARY(T, N)                                 \
        extern T d_##N;                               \
                                                      \
        T x_##N;                                      \
        T xa_##N[ARRAY];                              \
        T xm_##N[ROWS][COLUMNS];                      \
        T *xp_##N;                                    \
                                                      \
        long touch_##N(T *p, long v)                  \
        {                                             \
                long old = *p;                        \
                                                      \
                *p = (T) v;                           \
                return old;                           \
        }                                             \
                                                      \
        long fill_##N(T *a, int n, long v)            \
        {                                             \
                long sum = 0;                         \
                                                      \
                for (int i = 0; i < n; ++i) {         \
                        sum += a[i];                  \
                        a[i] = (T) (v + i);           \
                }                                     \
                return sum;                           \
        }                                             \
                                                      \
        long total_##N(T *a, int n)                   \
        {                                             \
                long sum = 0;                         \
                                                      \
                for (int i = 0; i < n; ++i)           \
                        sum += a[i] * (long) (i + 1); \
                return sum;                           \
        }                                             \
                                                      \
        void poke_d_##N(long v)                       \
        {                                             \
                d_##N = (T) v;                        \
        }                                             \
        void poke_x_##N(long v)                       \
        {                                             \
                x_##N = (T) v;                        \
        }

TYPES(LIBRARY)
