/* The lists the walk probe is generated from.  walk.c makes functions
   of each entry, compiled by rcc and, as the reference, by clang -O0
   under the prefix ref_.  walk-lib.c holds the functions they call,
   compiled by clang alone and linked under both names.  walk-main.c
   calls both builds over a table of values and compares the results
   and the bytes of the memory they wrote.  An access through a
   register pointer stepped by ++, -- or a compound assignment by a
   constant: *p++, *p--, *++p, *--p, *(p += c) and *(p -= c), as a
   load, as a store and as both, docs/aarch64.md 2.3 and 6.3. */

/* The pointees of a walk: the type and the letter it takes in a
   function's name.  Sizes 1, 2, 4 and 8, each in both signednesses
   where the load's extension tells them apart, and a pointer. */
#define INTEGERS(X)                                                     \
    X(char, c) X(signed char, sc) X(short, s) X(unsigned short, us)     \
    X(int, i) X(unsigned, u) X(long, l)

#define POINTEES(X) INTEGERS(X) X(char *, p)

/* A constant c of *(p += c) and *(p -= c) on each pointee: products of
   1, 2 and 7 times the size, which the pre-indexed form encodes. */
#define NEAR(X, T, N) X(T, N, 1) X(T, N, 2) X(T, N, 7)

/* Products at the edge of the form's reach, -256 to 255 bytes, and
   past it: 255 and 256 on a char, 300 on a char, 64 on an int, whose
   -256 is in reach and 256 is not, and 33 on a long, 264 either way. */
#define FAR(X)                                                          \
    X(char, c, 255) X(char, c, 256) X(char, c, 300) X(int, i, 64)       \
    X(long, l, 33)
