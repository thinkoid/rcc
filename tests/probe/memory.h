/* The lists the memory probe is generated from.  memory.c makes
   functions of each entry, compiled by rcc and, as the reference, by
   clang -O0 under the prefix ref_.  memory-lib.c holds the functions
   they call, compiled by clang alone and linked under both names.
   memory-main.c calls both builds over a table of values and compares
   the results and the bytes of the memory they wrote.  Addresses and
   indirection, docs/aarch64.md 6.3's rows from &x to "abc" but those
   of pointer arithmetic and the subscript, which are the pointers
   probe's; the frame of 5.3, depth of 6.2, G7, G8, G10 and G11 of
   chapter 9 and the literals of chapter 10. */

/* the integer types and the letter each takes in a function's name */
#define NARROW(X)                                                       \
    X(char, c) X(signed char, sc) X(unsigned char, uc)                  \
    X(short, s) X(unsigned short, us)

#define WIDE(X) X(int, i) X(unsigned, u) X(long, l) X(unsigned long, ul)

#define TYPES(X) NARROW(X) WIDE(X)

/* eight long parameters, the convention's registers on aarch64; a
   ninth and a tenth go on the stack on both targets */
#define P8(T) T a, T b, T c, T d, T e, T f, T g, T h

/* a compound assignment through a pointer: a name and the operator.
   The shifts are apart, since their count is an int whatever the
   object's type. */
#define COMPOUNDS(X, T, N)                                              \
    X(T, N, add, +=) X(T, N, sub, -=) X(T, N, mul, *=)                  \
    X(T, N, div, /=) X(T, N, rem, %=) X(T, N, and, &=)                  \
    X(T, N, or, |=) X(T, N, xor, ^=)

#define SHIFTS(X, T, N) X(T, N, shl, <<=) X(T, N, shr, >>=)

/* a compound assignment of a constant: a name, the operator, the
   constant; an immediate the instruction takes, one it does not, and
   the operators that take none */
#define KCOMPOUNDS(X, T, N)                                             \
    X(T, N, add_1, +=, 1) X(T, N, add_4096, +=, 4096)                   \
    X(T, N, sub_5000, -=, 5000) X(T, N, add_m1, +=, -1)                 \
    X(T, N, add_big, +=, 0x123456789) X(T, N, and_ff, &=, 0xff)         \
    X(T, N, or_m256, |=, -256) X(T, N, xor_12345, ^=, 0x12345)          \
    X(T, N, mul_3, *=, 3) X(T, N, div_7, /=, 7) X(T, N, rem_10, %=, 10) \
    X(T, N, shl_3, <<=, 3) X(T, N, shr_1, >>=, 1)

/* a constant stored through a pointer, converted to the object's type
   by the parser: zero, small ones, and those a mov does not hold
   alone, which take a movk on aarch64 at 32 and at 64 bits */
#define STORES(X, T, N)                                                 \
    X(T, N, 0, 0) X(T, N, 5, 5) X(T, N, m1, -1) X(T, N, ff, 0xff)       \
    X(T, N, 7fff, 0x7fff) X(T, N, 12345, 0x12345)                       \
    X(T, N, 89abcdef, 0x89abcdef) X(T, N, m100000, -100000)             \
    X(T, N, high, 0x1234000000000000) X(T, N, all, 0x123456789abcdef0)
