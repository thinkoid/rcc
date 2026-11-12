/* The lists the inplace probe is generated from.  inplace.c makes
   functions of each entry, compiled by rcc and, as the reference, by
   clang -O0 under the prefix ref_.  inplace-lib.c holds the functions
   they call, compiled by clang alone and linked under both names.
   inplace-main.c calls both builds over a table of values and compares
   the results and the bytes of the memory they wrote.  Compound
   assignment and ++ and -- on a register variable, in place,
   docs/aarch64.md chapter 4 and backend.md chapters 4 and 6.7. */

/* the integer types and the letter each takes in a function's name */
#define NARROW(X)                                                       \
    X(char, c) X(signed char, sc) X(unsigned char, uc)                  \
    X(short, s) X(unsigned short, us)

#define WIDE(X) X(int, i) X(unsigned, u) X(long, l) X(unsigned long, ul)

#define TYPES(X) NARROW(X) WIDE(X)

/* The compound assignments: a name, the operator, a constant the
   instruction encodes and one it does not.  4095 is an add and sub
   immediate on aarch64 and 0xff a logical one; 0x12345 is neither,
   and x86-64 encodes both.  mul takes no immediate on aarch64. */
#define COMPOUNDS(X, T, N)                                              \
    X(T, N, add, +=, 4095, 0x12345) X(T, N, sub, -=, 4095, 0x12345)     \
    X(T, N, mul, *=, 3, 0x12345) X(T, N, and, &=, 0xff, 0x12345)        \
    X(T, N, or, |=, 0xff, 0x12345) X(T, N, xor, ^=, 0xff, 0x12345)

/* the divisions, whose register and slot divisors the main keeps from
   0 and -1 */
#define DIVISIONS(X, T, N)                                              \
    X(T, N, div, /=, 7, 0x12345) X(T, N, rem, %=, 7, 0x12345)

/* the shifts, whose count the main keeps below 32 */
#define SHIFTS(X, T, N) X(T, N, shl, <<=) X(T, N, shr, >>=)

/* The pointees of a stepped pointer: a name, a type and the declarator
   suffix that makes a parameter a pointer to it.  Sizes 1, 2, 4 and 8,
   and a row of 12 bytes, which no addressing form scales by. */
#define POINTEES(X)                                                     \
    X(c, char, []) X(s, short, []) X(i, int, []) X(l, long, [])         \
    X(r12, int, [][3])

/* the scalar pointees alone, for the loads and stores of a walk */
#define SCALARS(X) X(c, char) X(s, short) X(i, int) X(l, long)

/* eight long parameters */
#define P8(T) T a, T b, T c, T d, T e, T f, T g, T h
