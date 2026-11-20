/* The lists the calls probe is generated from.  calls.c makes
   functions of each entry, compiled by rcc and, as the reference, by
   clang -O0 under the prefix ref_.  calls-lib.c holds the functions
   they call, compiled by clang alone and linked under both names.
   calls-main.c calls both builds over a table of values and compares.
   Calls in the three pairings of calls.c, docs/aarch64.md chapter 8. */

/* the integer types and the letter each takes in a function's name */
/* clang-format off */
#define NARROW(X)                                                       \
    X(char, c) X(signed char, sc) X(unsigned char, uc)                  \
    X(short, s) X(unsigned short, us)
/* clang-format on */

#define WIDE(X) X(int, i) X(unsigned, u) X(long, l) X(unsigned long, ul)

#define TYPES(X) NARROW(X) WIDE(X)

/* parameter lists of one type, and the sum that tells the positions
   apart */
#define P8(T)  T a, T b, T c, T d, T e, T f, T g, T h
#define P9(T)  P8(T), T i
#define P10(T) P9(T), T j
#define P11(T) P10(T), T k
#define P12(T) P11(T), T l

#define S1  a * 3L
#define S2  S1 + b * 5L
#define S3  S2 + c * 7L
#define S4  S3 + d * 11L
#define S5  S4 + e * 13L
#define S6  S5 + f * 17L
#define S7  S6 + g * 19L
#define S8  S7 + h * 23L
#define S9  S8 + i * 29L
#define S10 S9 + j * 31L
#define S11 S10 + k * 37L
#define S12 S11 + l * 41L

/* twelve parameters of mixed types, the ninth to the twelfth on the
   stack on aarch64, the seventh to the twelfth on x86-64 */
#define MIXED12                                                            \
        char a, short b, int c, long d, unsigned char e, unsigned short f, \
            unsigned g, unsigned long h, signed char i, short j, char k, int l

/* The many-argument calls, chapter 8: an outgoing area whose last
   stores lie past the reach of a store's offset, 4095 bytes for strb,
   8190 for strh, 16380 for str of a w register and 32760 of an x
   register, so they go through x16 formed from sp.  MANY_TYPES gives
   each width's letter and the block of its arguments: many_<n> takes
   a long, the block, and sixteen more, all of the width, the last
   stacked one at 4160, 8256, 16448 or 32832 bytes.  W<k>(X, p) is X
   applied to k names, p followed by the octal digits of each
   position, so with p 0 it is X applied to the octal constants 0 to
   k - 1. */
/* clang-format off */
#define W8(X, p)                                                        \
    X(p##0) X(p##1) X(p##2) X(p##3) X(p##4) X(p##5) X(p##6) X(p##7)
#define W64(X, p)                                                       \
    W8(X, p##0) W8(X, p##1) W8(X, p##2) W8(X, p##3)                     \
    W8(X, p##4) W8(X, p##5) W8(X, p##6) W8(X, p##7)
#define W512(X, p)                                                      \
    W64(X, p##0) W64(X, p##1) W64(X, p##2) W64(X, p##3)                 \
    W64(X, p##4) W64(X, p##5) W64(X, p##6) W64(X, p##7)
/* clang-format on */
#define W1024(X, p) W512(X, p##0) W512(X, p##1)
#define W2048(X, p) W512(X, p##0) W512(X, p##1) W512(X, p##2) W512(X, p##3)
/* clang-format off */
#define W4096(X, p)                                                     \
    W512(X, p##0) W512(X, p##1) W512(X, p##2) W512(X, p##3)             \
    W512(X, p##4) W512(X, p##5) W512(X, p##6) W512(X, p##7)
/* clang-format on */

#define MANY_TYPES(X) X(sc, W512) X(s, W1024) X(i, W2048) X(l, W4096)

#define MANY_PARAMETER_sc(p) , signed char p
#define MANY_PARAMETER_s(p)  , short p
#define MANY_PARAMETER_i(p)  , int p
#define MANY_PARAMETER_l(p)  , long p

/* clang-format off */
#define MANY_PARAMETERS(N, BLOCK)                                       \
    long n BLOCK(MANY_PARAMETER_##N, a) W8(MANY_PARAMETER_##N, b)       \
        W8(MANY_PARAMETER_##N, c)
/* clang-format on */
