/* The copies probe's main, compiled by clang -O0: calls every function
   of copies.c as rcc compiled it and as clang did (ref_), each on
   arrays of three objects of its own, a destination and a source,
   filled alike, and compares the bytes each build leaves in its
   destination, the objects and a guard of bytes on each side, and in
   its source, which no copy writes.  Prints each difference and a
   summary; exits 0 when nothing differs, 1 otherwise. */

#include <stdio.h>
#include <string.h>

#include "copies.h"

/* the bytes around the objects, a multiple of every alignment */
#define GUARD 16

/* the bytes of three objects of the largest record and the guards */
#define BYTES (GUARD + 3 * 64 + GUARD)

/* the bytes of three objects of T and the guards */
#define SPAN(T) (GUARD + 3 * sizeof(T) + GUARD)

/* the objects of T in a buffer, past its first guard */
#define OBJECTS(T, BUFFER) ((T *) ((BUFFER) + GUARD))

/* rcc's buffers, [0], and clang's, [1] */
static _Alignas(16) unsigned char to[2][BYTES];
static _Alignas(16) unsigned char from[2][BYTES];

static long total;
static long bad;

static void
report(const char *name, unsigned seed, int differs)
{
        ++total;

        if (!differs)
                return;

        if (++bad <= 40)
                printf("%s, seed %u: the bytes differ\n", name, seed);
}

/* bytes, each its own value of seed */
static void
fill(unsigned char *p, size_t size, unsigned seed)
{
        for (size_t i = 0; i < size; ++i)
                p[i] = (unsigned char) (i * 29 + seed * 101 + 7);
}

/* both builds' buffers filled alike, the sources apart from the
   destinations */
static void
prepare(unsigned seed)
{
        fill(to[0], BYTES, seed);
        fill(from[0], BYTES, seed + 1);
        memcpy(to[1], to[0], BYTES);
        memcpy(from[1], from[0], BYTES);
}

/* the bytes both builds left, compared over span */
static void
compare(const char *name, unsigned seed, size_t span)
{
        report(name, seed,
               memcmp(to[0], to[1], span) != 0 ||
                   memcmp(from[0], from[1], span) != 0);
}

/* each function of copies.c, as rcc compiled it and as clang did */
#define DECLARE(K, N)                                                   \
    void copy_##N(K N *, K N *), ref_copy_##N(K N *, K N *);            \
    void element_##N(K N *, K N *, int);                                \
    void ref_element_##N(K N *, K N *, int);                            \
    void local_##N(K N *, K N *), ref_local_##N(K N *, K N *);

#define SIZED_DECLARE(N, T, COUNT) DECLARE(struct, N)
#define PADDED_DECLARE(N, MEMBERS) DECLARE(struct, N)
#define UNION_DECLARE(N, MEMBERS) DECLARE(union, N)

SIZED(SIZED_DECLARE)
PADDED(PADDED_DECLARE)
UNIONS(UNION_DECLARE)

/* one function F of the record T, by both builds, on the buffers
   filled for seed, its arguments after the two arrays the rest */
#define CALL(F, T, seed, ...)                                           \
    prepare(seed);                                                      \
    F(OBJECTS(T, to[0]), OBJECTS(T, from[0]) __VA_ARGS__);              \
    ref_##F(OBJECTS(T, to[1]), OBJECTS(T, from[1]) __VA_ARGS__);        \
    compare(#F, seed, SPAN(T));

#define CHECK(K, N)                                                     \
    for (unsigned seed = 0; seed < 2; ++seed) {                         \
        CALL(copy_##N, K N, seed, )                                     \
        CALL(element_##N, K N, seed, , 1)                               \
        CALL(local_##N, K N, seed, )                                    \
    }

#define SIZED_CHECK(N, T, COUNT) CHECK(struct, N)
#define PADDED_CHECK(N, MEMBERS) CHECK(struct, N)
#define UNION_CHECK(N, MEMBERS) CHECK(union, N)

int
main(void)
{
        SIZED(SIZED_CHECK)
        PADDED(PADDED_CHECK)
        UNIONS(UNION_CHECK)

        printf("%ld checks, %ld differ\n", total, bad);

        return bad != 0;
}
