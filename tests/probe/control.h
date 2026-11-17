/* The lists the control probe is generated from.  control.c makes
   functions of each entry, compiled by rcc and, as the reference, by
   clang -O0 under the prefix ref_; control-main.c calls both over a
   table of values and compares.  Conditions and control,
   docs/aarch64.md chapter 7: no call and no pointer, but as the right
   operand of a comparison with a constant on the left, docs/backend.md
   6.1. */

/* the integer types and the letter each takes in a function's name */
#define TYPES(X)                                                        \
    X(char, c) X(signed char, sc) X(unsigned char, uc)                  \
    X(short, s) X(unsigned short, us)                                   \
    X(int, i) X(unsigned, u) X(long, l) X(unsigned long, ul)

/* the types a constant operand is tried on */
#define WIDE(X) X(int, i) X(unsigned, u) X(long, l) X(unsigned long, ul)

/* a comparison with a constant: a name, the operator, the constant.
   The constants walk the immediates cmp encodes and those it does not,
   docs/aarch64.md 2.4 and 6.4. */
#define KCMPS(X, T, N)                                                  \
    X(T, N, lt_5, <, 5) X(T, N, eq_m1, ==, -1) X(T, N, gt_4095, >, 4095) \
    X(T, N, ge_4096, >=, 4096) X(T, N, ne_5000, !=, 5000)               \
    X(T, N, le_12345, <=, 0x12345) X(T, N, eq_0, ==, 0)                 \
    X(T, N, ne_0, !=, 0) X(T, N, lt_0, <, 0) X(T, N, ge_m4096, >=, -4096) \
    X(T, N, lt_fff000, <, 0xfff000) X(T, N, gt_m5000, >, -5000)

/* a comparison of an object in memory with a constant, which x86-64
   writes as the cmp of the object itself as a condition, docs/backend.md
   7.1: a name, the operator, the constant.  Each operator, with
   constants an int holds and one it does not, positive and negative,
   so an unsigned type compares a large value. */
#define KSIDES(X, T, N)                                                 \
    X(T, N, lt_5, <, 5) X(T, N, le_5, <=, 5) X(T, N, gt_5, >, 5)        \
    X(T, N, ge_5, >=, 5) X(T, N, eq_5, ==, 5) X(T, N, ne_5, !=, 5)      \
    X(T, N, lt_0, <, 0) X(T, N, ge_0, >=, 0) X(T, N, eq_0, ==, 0)       \
    X(T, N, le_m1, <=, -1) X(T, N, gt_m1, >, -1) X(T, N, ne_m1, !=, -1) \
    X(T, N, lt_m4096, <, -4096) X(T, N, ge_12345, >=, 0x12345)          \
    X(T, N, gt_7fff, >, 0x7fff) X(T, N, le_big, <=, 0x123456789)

/* The conditions that jump on the flags of their operator's own
   instruction on x86-64, docs/backend.md 7.1, and beside them those
   that keep the cmp: a name and the condition.  Those of a and b alone:
   +, -, |, ^ and unary -, & of two registers and of an immediate, ==
   0 and != 0 over them; then a product, a shift and < 0. */
#define CC_VALUES(X, T, N)                                              \
    X(T, N, add, a + b) X(T, N, sub, a - b) X(T, N, or, a | b)          \
    X(T, N, xor, a ^ b) X(T, N, neg, -a) X(T, N, and, a & b)            \
    X(T, N, and_imm, a & 0x10081) X(T, N, add_eq, (a + b) == 0)         \
    X(T, N, sub_ne, (a - b) != 0) X(T, N, and_eq, (a & b) == 0)         \
    X(T, N, xor_ne, (a ^ b) != 0) X(T, N, neg_eq, -a == 0)              \
    X(T, N, mul, a * b) X(T, N, shl, a << 1) X(T, N, add_lt, (a + b) < 0)

/* Those on an object O: each compound assignment that has a memory
   form, a prefix ++ and --, == 0 and != 0 over them, and & with the
   object on the left; then a postfix ++, whose value is the old one. */
#define CC_OBJECT(X, T, N, K, O)                                        \
    X(T, N, K, O, add_to, O += b) X(T, N, K, O, sub_from, O -= b)       \
    X(T, N, K, O, and_to, O &= b) X(T, N, K, O, or_to, O |= b)          \
    X(T, N, K, O, xor_to, O ^= b) X(T, N, K, O, inc, ++O)               \
    X(T, N, K, O, dec, --O) X(T, N, K, O, add_to_eq, (O += b) == 0)     \
    X(T, N, K, O, dec_ne, (--O) != 0) X(T, N, K, O, and_object, O & b)  \
    X(T, N, K, O, post_inc, O++)

/* the least and the greatest char, whose signedness is the target's */
#define CHAR_LEAST ((char) -1 < 0 ? -128 : 0)
#define CHAR_MOST ((char) -1 < 0 ? 127 : 255)

/* The types a comparison with the constant on the left is tried on,
   docs/backend.md 6.1: the type, the letter of its name, its least and
   its greatest value.  The pointer is tried apart, its constants cast
   to it. */
#define LEFT_TYPES(X)                                                   \
    X(char, c, CHAR_LEAST, CHAR_MOST) X(unsigned char, uc, 0, 255)      \
    X(short, s, -32768, 32767) X(unsigned short, us, 0, 65535)          \
    X(int, i, -2147483647 - 1, 2147483647)                              \
    X(unsigned, u, 0, 4294967295u)                                      \
    X(long, l, -9223372036854775807L - 1, 9223372036854775807L)         \
    X(unsigned long, ul, 0, 18446744073709551615ul)

/* each of the six comparisons with the constant k on the left */
#define KLEFT_OPS(X, T, N, name, k)                                     \
    X(T, N, lt_##name, <, k) X(T, N, le_##name, <=, k)                  \
    X(T, N, gt_##name, >, k) X(T, N, ge_##name, >=, k)                  \
    X(T, N, eq_##name, ==, k) X(T, N, ne_##name, !=, k)

/* the constants on the left: negative, zero, positive, one aarch64's
   cmp does not encode, and the type's least and greatest */
#define KLEFTS(X, T, N, least, most)                                    \
    KLEFT_OPS(X, T, N, m5, -5) KLEFT_OPS(X, T, N, 0, 0)                 \
    KLEFT_OPS(X, T, N, 5, 5) KLEFT_OPS(X, T, N, 5000, 5000)             \
    KLEFT_OPS(X, T, N, min, least) KLEFT_OPS(X, T, N, max, most)
