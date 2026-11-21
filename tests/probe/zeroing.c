/* The zeroing probe: a local record initialized by a list in rcc's C,
   6.7.11p17 to p22, on records of every size from 1 to 64 bytes at
   each alignment from 1 to 8 and past eight units of 8, records with
   padding and unions: the first scalar listed, every scalar listed, the
   empty list, and lists of designators, which give a scalar a value
   that a later one overrides, 6.7.11p20: a record's every scalar twice,
   and an array's last element before its first, or before a braced
   list of the array, which zeroes it again.  Each function reaches its
   declaration twice in one activation, and so in one slot (G12): the
   library's taken() keeps the local's bytes and then fills the local
   with 0xa5, so the second zeroing finds no zero byte in the slot, and
   the bytes kept are the second's.  See zeroing.h. */

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

/* a scalar's two values in a list of designators, the second
   overriding the first, 6.7.11p20 */
#undef F
#define F(PATH, ADDED) .PATH = v, .PATH = v + ADDED,

#define SIZED_DESIGNATED(N, T, COUNT)                                   \
        LISTED(struct, N, designated, .m[COUNT - 1] = v, .m[0] = v + 1) \
        LISTED(struct, N, rebraced, .m[COUNT - 1] = v, .m = { v + 1 })

#define PADDED_DESIGNATED(N, MEMBERS, SCALARS) \
        LISTED(struct, N, designated, SCALARS)

#define UNION_DESIGNATED(N, MEMBERS, SCALARS) \
        LISTED(union, N, designated, SCALARS)

SIZED(SIZED_DESIGNATED)
PADDED(PADDED_DESIGNATED)
UNIONS(UNION_DESIGNATED)
