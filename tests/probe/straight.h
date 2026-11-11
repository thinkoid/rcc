/* The lists the probe is generated from.  straight.c makes a function of
   each entry, compiled by rcc and, as the reference, by clang -O0 under
   the prefix ref_; straight-main.c calls both over a table of values and
   compares.  Straight-line code only, docs/aarch64.md chapters 5 and
   6: no call, no branch, no pointer. */

/* the integer types and the letter each takes in a function's name */
#define TYPES(X)                                                        \
    X(char, c) X(signed char, sc) X(unsigned char, uc)                  \
    X(short, s) X(unsigned short, us)                                   \
    X(int, i) X(unsigned, u) X(long, l) X(unsigned long, ul)

/* the same list under a second name, for the pairs of the cast matrix */
#define TYPES2(X, F, FN)                                                \
    X(F, FN, char, c) X(F, FN, signed char, sc) X(F, FN, unsigned char, uc) \
    X(F, FN, short, s) X(F, FN, unsigned short, us)                     \
    X(F, FN, int, i) X(F, FN, unsigned, u) X(F, FN, long, l)            \
    X(F, FN, unsigned long, ul)

/* the types a constant operand is tried on */
#define WIDE(X) X(int, i) X(unsigned, u) X(long, l) X(unsigned long, ul)

/* an operator with a constant right operand: a name, the operator,
   the constant.  The constants walk the immediates of docs/aarch64.md
   2.4 and 6.4: those add, sub and the logical instructions encode,
   those they do not, the negative ones, and the instructions that take
   none. */
#define KONSTS(X, T, N)                                                 \
    X(T, N, add_0, +, 0) X(T, N, add_1, +, 1) X(T, N, add_4095, +, 4095) \
    X(T, N, add_4096, +, 4096) X(T, N, add_4097, +, 4097)               \
    X(T, N, add_5000, +, 5000) X(T, N, add_fff000, +, 0xfff000)         \
    X(T, N, add_1000000, +, 0x1000000) X(T, N, add_m1, +, -1)           \
    X(T, N, add_m4095, +, -4095) X(T, N, add_m4096, +, -4096)           \
    X(T, N, add_m5000, +, -5000)                                        \
    X(T, N, sub_1, -, 1) X(T, N, sub_4096, -, 4096)                     \
    X(T, N, sub_5000, -, 5000) X(T, N, sub_m1, -, -1)                   \
    X(T, N, and_ff, &, 0xff) X(T, N, and_5, &, 5)                       \
    X(T, N, and_ff00, &, 0xff00) X(T, N, and_5555, &, 0x55555555)       \
    X(T, N, and_m256, &, -256) X(T, N, and_0, &, 0)                     \
    X(T, N, or_ff, |, 0xff) X(T, N, or_5, |, 5) X(T, N, or_m1, |, -1)   \
    X(T, N, xor_ff, ^, 0xff) X(T, N, xor_5, ^, 5) X(T, N, xor_m1, ^, -1) \
    X(T, N, mul_3, *, 3) X(T, N, mul_4, *, 4) X(T, N, mul_m1, *, -1)    \
    X(T, N, div_4, /, 4) X(T, N, div_7, /, 7) X(T, N, div_m3, /, -3)    \
    X(T, N, rem_4, %, 4) X(T, N, rem_10, %, 10)                         \
    X(T, N, shl_0, <<, 0) X(T, N, shl_1, <<, 1) X(T, N, shl_31, <<, 31) \
    X(T, N, shr_1, >>, 1) X(T, N, shr_31, >>, 31)

/* the same for the 64-bit types only */
#define KONSTS64(X, T, N)                                               \
    X(T, N, add_big, +, 0x123456789abcdef) X(T, N, add_4g, +, 0x100000000) \
    X(T, N, add_m100000, +, -100000L) X(T, N, and_big, &, 0xffff0000ffff0000) \
    X(T, N, and_low32, &, 0xffffffff) X(T, N, or_big, |, 0x8000000000000001) \
    X(T, N, shl_63, <<, 63) X(T, N, shl_32, <<, 32) X(T, N, shr_33, >>, 33)

/* a constant that must be held, docs/aarch64.md 6.4: a name, the type
   and the value */
#define HELD(X)                                                         \
    X(h0, int, 0) X(h1, int, 1) X(hm1, int, -1) X(h65535, int, 65535)   \
    X(h65536, int, 65536) X(h12340000, int, 0x12340000)                 \
    X(h12345678, int, 0x12345678) X(h7fffffff, int, 0x7fffffff)         \
    X(hmin, int, -2147483647 - 1) X(hm100000, int, -100000)             \
    X(hfffe0000, unsigned, 0xfffe0000) X(hffffffff, unsigned, 0xffffffff) \
    X(h00ff00ff, unsigned, 0x00ff00ff) X(hl0, long, 0L)                 \
    X(hlm1, long, -1L) X(hl4g, long, 0x100000000)                       \
    X(hlhigh, long, 0x1234567800000000) X(hlall, long, 0x123456789abcdef0) \
    X(hlm100000, long, -100000L) X(hlmask, unsigned long, 0xffff0000ffff0000) \
    X(hlmid, long, 0x0000ffff00000000) X(hlneg3, long, -0x123456789abL) \
    X(hlmin, long, -9223372036854775807L - 1) X(hc, char, 200)          \
    X(hsc, signed char, -100) X(hs, short, -30000) X(hus, unsigned short, 60000)
