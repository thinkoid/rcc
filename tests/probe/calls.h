/* The lists the calls probe is generated from.  calls.c makes
   functions of each entry, compiled by rcc and, as the reference, by
   clang -O0 under the prefix ref_.  calls-lib.c holds the functions
   they call, compiled by clang alone and linked under both names.
   calls-main.c calls both builds over a table of values and compares.
   Calls in the three pairings of calls.c, docs/aarch64.md chapter 8. */

/* the integer types and the letter each takes in a function's name */
#define NARROW(X)                                                       \
    X(char, c) X(signed char, sc) X(unsigned char, uc)                  \
    X(short, s) X(unsigned short, us)

#define WIDE(X) X(int, i) X(unsigned, u) X(long, l) X(unsigned long, ul)

#define TYPES(X) NARROW(X) WIDE(X)

/* parameter lists of one type, and the sum that tells the positions
   apart */
#define P8(T) T a, T b, T c, T d, T e, T f, T g, T h
#define P9(T) P8(T), T i
#define P10(T) P9(T), T j
#define P11(T) P10(T), T k
#define P12(T) P11(T), T l

#define S1 a * 3L
#define S2 S1 + b * 5L
#define S3 S2 + c * 7L
#define S4 S3 + d * 11L
#define S5 S4 + e * 13L
#define S6 S5 + f * 17L
#define S7 S6 + g * 19L
#define S8 S7 + h * 23L
#define S9 S8 + i * 29L
#define S10 S9 + j * 31L
#define S11 S10 + k * 37L
#define S12 S11 + l * 41L

/* twelve parameters of mixed types, the ninth to the twelfth on the
   stack on aarch64, the seventh to the twelfth on x86-64 */
#define MIXED12                                                         \
    char a, short b, int c, long d, unsigned char e, unsigned short f,  \
    unsigned g, unsigned long h, signed char i, short j, char k, int l
