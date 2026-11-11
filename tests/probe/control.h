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
