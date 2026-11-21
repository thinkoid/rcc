/* The zeroing probe: a local record or array initialized by a list in
   rcc's C, 6.7.11p17 to p22, on records of every size from 1 to 64
   bytes at each alignment from 1 to 8 and past eight units of 8,
   records with padding and unions: the first scalar listed, every
   scalar listed, the empty list, and lists of designators, which give a
   scalar a value that a later one overrides, 6.7.11p20: a record's
   every scalar twice, and an array's last element before its first, or
   before a braced list of the array, which zeroes it again.  The arrays
   are a record's array member as a local of its own, and an array of
   two records with padding, every scalar of both listed or the second's
   alone.  Each function reaches its declaration twice in one
   activation, and so in one slot (G12): the library's taken() keeps the
   local's bytes and then fills the local with 0xa5, so the second
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

/* the function F of a local array of ELEMENTS of T for record N, its
   list the rest */
#define ARRAY_LISTED(T, ELEMENTS, N, F, ...)             \
        void F##_##N(T *to, long v)                      \
        {                                                \
                for (int pass = 0; pass < 2; ++pass) {   \
                        T t[ELEMENTS] = { __VA_ARGS__ }; \
                                                         \
                        taken(to, t, sizeof t);          \
                }                                        \
        }

/* a scalar's value in a list of every scalar */
#undef F
#define F(PATH, ADDED) v + ADDED,

/* the array of a record's member, of its element type, and an array of
   two records with padding */
#define SIZED_ARRAY(N, T, COUNT)                    \
        ARRAY_LISTED(T, COUNT, N, array_first, v)   \
        ARRAY_LISTED(T, COUNT, N, array_empty, )    \
        ARRAY_LISTED(T, COUNT, N, array_designated, \
                     [COUNT - 1] = v, [0] = v + 1)

#define PADDED_ARRAY(N, MEMBERS, SCALARS)                         \
        ARRAY_LISTED(struct N, 2, N, array_full, SCALARS SCALARS) \
        ARRAY_LISTED(struct N, 2, N, array_second, [1] = { SCALARS })

SIZED(SIZED_ARRAY)
PADDED(PADDED_ARRAY)
