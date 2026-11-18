/* The copies probe: a record's assignment in rcc's C, 6.5.17.2, on
   records of every size from 1 to 64 bytes at each alignment from 1 to
   8, records with padding and unions: through two pointers, between
   two elements of arrays by a variable index, and through a local, the
   copy into the local's slot and the copy out of it, the local
   assigned or initialized.  See copies.h. */

#include "copies.h"

/* the functions of record N declared by K: *to = *from, to[i] =
   from[i + 1], and the copy through a local, assigned and initialized */
#define COPIES(K, N)                                                    \
    void copy_##N(K N *to, K N *from) { *to = *from; }                  \
    void element_##N(K N *to, K N *from, int i)                         \
    {                                                                   \
        to[i] = from[i + 1];                                            \
    }                                                                   \
    void local_##N(K N *to, K N *from)                                  \
    {                                                                   \
        K N t;                                                          \
                                                                        \
        t = *from;                                                      \
        *to = t;                                                        \
    }                                                                   \
    void initialized_##N(K N *to, K N *from)                            \
    {                                                                   \
        K N t = *from;                                                  \
                                                                        \
        *to = t;                                                        \
    }

#define SIZED_COPIES(N, T, COUNT) COPIES(struct, N)
#define PADDED_COPIES(N, MEMBERS) COPIES(struct, N)
#define UNION_COPIES(N, MEMBERS) COPIES(union, N)

SIZED(SIZED_COPIES)
PADDED(PADDED_COPIES)
UNIONS(UNION_COPIES)
