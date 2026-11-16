/* The lists the control probe is generated from.  control.c makes
   functions of each entry, compiled by rcc and, as the reference, by
   clang -O0 under the prefix ref_; control-main.c calls both over a
   table of values and compares.  Conditions and control,
   docs/aarch64.md chapter 7: no call, no pointer. */

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
