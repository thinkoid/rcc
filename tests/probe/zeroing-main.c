/* The zeroing probe's main, compiled by clang -O0: calls every function
   of zeroing.c as rcc compiled it and as clang did (ref_), over a
   table of values, each build on an object of its own, and compares
   each scalar member of the two objects, and every byte of rcc's
   object outside its members against zero.  Prints each difference
   and a summary; exits 0 when nothing differs, 1 otherwise. */

#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "zeroing.h"

/* clang-format off */
static const long values[] = { 0,      1,           -1,
                               0x7f,   0x80,        0xff,
                               0x1234, -0x5678,     0x12345678,
                               -2,     0x123456789abcdef0L };
/* clang-format on */

#define COUNT ((int) (sizeof values / sizeof *values))

/* the bytes of the largest record */
#define BYTES 200

/* rcc's object, [0], and clang's, [1] */
static _Alignas(16) unsigned char objects[2][BYTES];

static long total;
static long bad;

/* a member of a record: its offset and its size in bytes */
struct span
{
        size_t offset;
        size_t size;
};

static void
report(const char *name, long v, const char *what, size_t at, int differs)
{
        ++total;

        if (!differs)
                return;

        if (++bad <= 40)
                printf("%s(%ld): %s at %zu differs\n", name, v, what, at);
}

/* the objects both builds left for v, of size bytes: each member of
   members compared, and every byte of rcc's object outside them
   against zero */
static void
compare(const char *name,
        long v,
        size_t size,
        const struct span *members,
        size_t count)
{
        unsigned char inside[BYTES] = { 0 };

        for (size_t k = 0; k < count; ++k) {
                const struct span m = members[k];

                report(name, v, "the member", m.offset,
                       memcmp(objects[0] + m.offset, objects[1] + m.offset,
                              m.size) != 0);
                memset(inside + m.offset, 1, m.size);
        }

        for (size_t i = 0; i < size; ++i)
                if (!inside[i])
                        report(name, v, "the padding byte", i,
                               objects[0][i] != 0);
}

/* each function of zeroing.c, as rcc compiled it and as clang did */
#define DECLARE(K, N, F) void F##_##N(K N *, long), ref_##F##_##N(K N *, long);

#define SIZED_DECLARE(N, T, COUNT) \
        DECLARE(struct, N, first)  \
        DECLARE(struct, N, empty)

#define PADDED_DECLARE(N, MEMBERS, SCALARS) \
        DECLARE(struct, N, first)           \
        DECLARE(struct, N, full)            \
        DECLARE(struct, N, empty)

#define UNION_DECLARE(N, MEMBERS, SCALARS) \
        DECLARE(union, N, first)           \
        DECLARE(union, N, full)            \
        DECLARE(union, N, empty)

SIZED(SIZED_DECLARE)
PADDED(PADDED_DECLARE)
UNIONS(UNION_DECLARE)

#define SIZED_DESIGNATED_DECLARE(N, T, COUNT) \
        DECLARE(struct, N, designated)        \
        DECLARE(struct, N, rebraced)

#define PADDED_DESIGNATED_DECLARE(N, MEMBERS, SCALARS) \
        DECLARE(struct, N, designated)

#define UNION_DESIGNATED_DECLARE(N, MEMBERS, SCALARS) \
        DECLARE(union, N, designated)

SIZED(SIZED_DESIGNATED_DECLARE)
PADDED(PADDED_DESIGNATED_DECLARE)
UNIONS(UNION_DESIGNATED_DECLARE)

/* one function F of record N declared by K, by both builds, for each
   value, each object filled with a byte no zeroing writes before the
   call */
#define CALL(K, N, F, SPANS)                                      \
        for (int i = 0; i < COUNT; ++i) {                         \
                memset(objects, 0x5a, sizeof objects);            \
                F##_##N((K N *) objects[0], values[i]);           \
                ref_##F##_##N((K N *) objects[1], values[i]);     \
                compare(#F "_" #N, values[i], sizeof(K N), SPANS, \
                        sizeof SPANS / sizeof *SPANS);            \
        }

/* a scalar member of the record named record_t, by its path */
#define F(PATH, ADDED) \
        { offsetof(record_t, PATH), sizeof(((record_t *) 0)->PATH) },

#define SIZED_CHECK(N, T, COUNT)                                  \
        {                                                         \
                typedef struct N record_t;                        \
                static const struct span members[] = { F(m, 0) }; \
                                                                  \
                CALL(struct, N, first, members)                   \
                CALL(struct, N, empty, members)                   \
        }

#define PADDED_CHECK(N, MEMBERS, SCALARS)                         \
        {                                                         \
                typedef struct N record_t;                        \
                static const struct span members[] = { SCALARS }; \
                                                                  \
                CALL(struct, N, first, members)                   \
                CALL(struct, N, full, members)                    \
                CALL(struct, N, empty, members)                   \
        }

#define UNION_CHECK(N, MEMBERS, SCALARS)                          \
        {                                                         \
                typedef union N record_t;                         \
                static const struct span members[] = { SCALARS }; \
                                                                  \
                CALL(union, N, first, members)                    \
                CALL(union, N, full, members)                     \
                CALL(union, N, empty, members)                    \
        }

#define SIZED_DESIGNATED_CHECK(N, T, COUNT)                       \
        {                                                         \
                typedef struct N record_t;                        \
                static const struct span members[] = { F(m, 0) }; \
                                                                  \
                CALL(struct, N, designated, members)              \
                CALL(struct, N, rebraced, members)                \
        }

#define PADDED_DESIGNATED_CHECK(N, MEMBERS, SCALARS)              \
        {                                                         \
                typedef struct N record_t;                        \
                static const struct span members[] = { SCALARS }; \
                                                                  \
                CALL(struct, N, designated, members)              \
        }

#define UNION_DESIGNATED_CHECK(N, MEMBERS, SCALARS)               \
        {                                                         \
                typedef union N record_t;                         \
                static const struct span members[] = { SCALARS }; \
                                                                  \
                CALL(union, N, designated, members)               \
        }

int
main(void)
{
        SIZED(SIZED_CHECK)
        PADDED(PADDED_CHECK)
        UNIONS(UNION_CHECK)
        SIZED(SIZED_DESIGNATED_CHECK)
        PADDED(PADDED_DESIGNATED_CHECK)
        UNIONS(UNION_DESIGNATED_CHECK)

        printf("%ld checks, %ld differ\n", total, bad);

        return bad != 0;
}
