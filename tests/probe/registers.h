/* The lists the registers probe is generated from.  registers.c makes
   functions of each entry, compiled by rcc and, as the reference, by
   clang -O0 under the prefix ref_.  registers-lib.c holds the
   functions they call, compiled by clang alone and linked under both
   names.  registers-main.c calls both builds over a table of values
   and compares the results and the bytes of the memory they wrote.
   Register variables as places, docs/aarch64.md chapter 4 and
   backend.md chapters 4 and 6.7: no compound assignment and no ++ or
   -- of a register variable, which are the inplace probe's. */

/* the integer types and the letter each takes in a function's name */
#define NARROW(X)                                                       \
    X(char, c) X(signed char, sc) X(unsigned char, uc)                  \
    X(short, s) X(unsigned short, us)

#define WIDE(X) X(int, i) X(unsigned, u) X(long, l) X(unsigned long, ul)

#define TYPES(X) NARROW(X) WIDE(X)

/* the types whose register claims more bits than the type has, G5 */
#define CLAIMED(X) NARROW(X) X(unsigned, u)

/* the same list under a second name, for the pairs of the cast matrix */
#define TYPES2(X, F, FN)                                                \
    X(F, FN, char, c) X(F, FN, signed char, sc) X(F, FN, unsigned char, uc) \
    X(F, FN, short, s) X(F, FN, unsigned short, us)                     \
    X(F, FN, int, i) X(F, FN, unsigned, u) X(F, FN, long, l)            \
    X(F, FN, unsigned long, ul)

/* The binary operators of the straight probe: a name, the operator, a
   constant the instruction encodes and one it does not.  4095 is an
   add, sub and cmp immediate on aarch64 and 0xff a logical one;
   0x12345 is neither, and x86-64 encodes both.  mul takes no
   immediate on aarch64. */
#define OPERATORS(X, T, N)                                              \
    X(T, N, add, +, 4095, 0x12345) X(T, N, sub, -, 4095, 0x12345)       \
    X(T, N, mul, *, 3, 0x12345) X(T, N, and, &, 0xff, 0x12345)          \
    X(T, N, or, |, 0xff, 0x12345) X(T, N, xor, ^, 0xff, 0x12345)        \
    X(T, N, lt, <, 4095, 0x12345) X(T, N, le, <=, 4095, 0x12345)        \
    X(T, N, gt, >, 4095, 0x12345) X(T, N, ge, >=, 4095, 0x12345)        \
    X(T, N, eq, ==, 4095, 0x12345) X(T, N, ne, !=, 4095, 0x12345)

/* the divisions, whose right operand the main keeps from 0 and -1 */
#define DIVISIONS(X, T, N)                                              \
    X(T, N, div, /, 7, 0x12345) X(T, N, rem, %, 7, 0x12345)

/* the shifts, whose count the main keeps below 32 */
#define SHIFTS(X, T, N) X(T, N, shl, <<) X(T, N, shr, >>)

/* the pointees of the pointer functions, sizes 1, 2, 4 and 8 */
#define SCALARS(X) X(c, char) X(s, short) X(i, int) X(l, long)

/* parameter lists of one type, and the sum that tells the positions
   apart */
#define P8(T) T a, T b, T c, T d, T e, T f, T g, T h
#define P12(T) P8(T), T i, T j, T k, T l

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

/* The same sum over the parameters as they promote: each is read at
   its promoted width before it is widened, so what a narrow one's
   register holds above the type's bits counts too. */
#define N12                                                             \
    ((a + 1) * 3L + (b + 1) * 5L + (c + 1) * 7L + (d + 1) * 11L +       \
     (e + 1) * 13L + (f + 1) * 17L + (g + 1) * 19L + (h + 1) * 23L +    \
     (i + 1) * 29L + (j + 1) * 31L + (k + 1) * 37L + (l + 1) * 41L)

/* Twelve parameters of one type, R the positions declared register:
   the first five, which are the five registers of x86-64; the sixth
   to the tenth, past x86-64's six argument registers from the
   seventh; the eleventh and twelfth, past aarch64's eight, mixed with
   ordinary ones; all twelve, more than either target has registers
   for. */
#define PARAMS(T, ra, rb, rc, rd, re, rf, rg, rh, ri, rj, rk, rl)       \
    ra T a, rb T b, rc T c, rd T d, re T e, rf T f, rg T g, rh T h,     \
    ri T i, rj T j, rk T k, rl T l

#define R register
#define PFIRST(T) PARAMS(T, R, R, R, R, R, , , , , , , )
#define PMIDDLE(T) PARAMS(T, , , , , , R, R, R, R, R, , )
#define PLAST(T) PARAMS(T, , R, , R, , , , , R, , R, R)
#define PALL(T) PARAMS(T, R, R, R, R, R, R, R, R, R, R, R, R)
