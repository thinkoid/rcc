/* The zeroing probe: a local record initialized by a list in rcc's C,
   6.7.11p17 to p22, on records of every size from 1 to 64 bytes at
   each alignment from 1 to 8 and past eight units of 8, records with
   padding and unions: the first scalar listed, every scalar listed, and
   the empty list.  Each function reaches its declaration twice in one
   activation, and so in one slot (G12): the library's taken() keeps
   the local's bytes and then fills the local with 0xa5, so the second
   zeroing finds no zero byte in the slot, and the bytes kept are the
   second's.  See zeroing.h. */

#include "zeroing.h"

/* the function F of record N declared by K, its list the rest */
#define LISTED(K, N, F, ...)                           \
        void F##_##N(K N *to, long v)                  \
        {                                              \
                for (int pass = 0; pass < 2; ++pass) { \
                        K N t = { __VA_ARGS__ };       \
                                                       \
                        taken(to, &t, sizeof t);       \
                }                                      \
        }

/* a scalar's value in a list of every scalar */
#define F(PATH, ADDED) v + ADDED,

#define SIZED_LISTED(N, T, COUNT)   \
        LISTED(struct, N, first, v) \
        LISTED(struct, N, empty, )

#define PADDED_LISTED(N, MEMBERS, SCALARS) \
        LISTED(struct, N, first, v)        \
        LISTED(struct, N, full, SCALARS)   \
        LISTED(struct, N, empty, )

#define UNION_LISTED(N, MEMBERS, SCALARS) \
        LISTED(union, N, first, v)        \
        LISTED(union, N, full, SCALARS)   \
        LISTED(union, N, empty, )

SIZED(SIZED_LISTED)
PADDED(PADDED_LISTED)
UNIONS(UNION_LISTED)
