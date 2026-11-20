/* The lists the memory probe is generated from.  memory.c makes
   functions of each entry, compiled by rcc and, as the reference, by
   clang -O0 under the prefix ref_.  memory-lib.c holds the functions
   they call, compiled by clang alone and linked under both names.
   memory-main.c calls both builds over a table of values and compares
   the results and the bytes of the memory they wrote.  Addresses and
   indirection, docs/aarch64.md 6.3's rows from &x to "abc" but those
   of pointer arithmetic and the subscript, which are the pointers
   probe's, but for the address of a local array or a literal plus a
   constant, backend.md 6.3's fold, a local array's element by a
   variable index, 6.3's slot and index, a constant over a row's
   address, summed into its displacement, and an object widened by its
   load, (T) a's row, an element by a subscript among them; an object
   in memory as an operator's right operand and as a condition's
   object, backend.md 6.1 and 7.1, by a name the file only declares,
   through a pointer and by a subscript, and the rows beside it: an
   object under a cast that keeps its bits, a comparison's value with a
   constant and a multiply by a constant; the frame of 5.3, depth of
   6.2, the pinned division and shift of 6.5 under each kind of node,
   G7, G8, G10 and G11 of chapter 9 and the literals of chapter 10. */

/* the integer types and the letter each takes in a function's name */
/* clang-format off */
#define NARROW(X)                                                       \
    X(char, c) X(signed char, sc) X(unsigned char, uc)                  \
    X(short, s) X(unsigned short, us)
/* clang-format on */

#define WIDE(X) X(int, i) X(unsigned, u) X(long, l) X(unsigned long, ul)

#define TYPES(X) NARROW(X) WIDE(X)

/* the types a read is widened to, crossed with one entry of TYPES, and
   the letter each takes in a function's name */
#define WIDENINGS(X, T, N) X(T, N, long, l) X(T, N, unsigned long, ul)

/* eight long parameters, the convention's registers on aarch64; a
   ninth and a tenth go on the stack on both targets */
#define P8(T) T a, T b, T c, T d, T e, T f, T g, T h

/* a compound assignment through a pointer: a name and the operator.
   The shifts are apart, since their count is an int whatever the
   object's type. */
/* clang-format off */
#define COMPOUNDS(X, T, N)                                              \
    X(T, N, add, +=) X(T, N, sub, -=) X(T, N, mul, *=)                  \
    X(T, N, div, /=) X(T, N, rem, %=) X(T, N, and, &=)                  \
    X(T, N, or, |=) X(T, N, xor, ^=)
/* clang-format on */

#define SHIFTS(X, T, N) X(T, N, shl, <<=) X(T, N, shr, >>=)

/* the types of the pinned rows: int, long and their unsigned types,
   and a char and a short beside them */
/* clang-format off */
#define PINNED_TYPES(X)                                                 \
    X(char, c) X(short, s) X(int, i) X(unsigned, u) X(long, l)          \
    X(unsigned long, ul)

/* the instructions that pin registers on x86-64, backend.md 6.5: a
   name, the operator and its compound form.  The main keeps a divisor
   from 0 and -1 and a count below the promoted width. */
#define PINNED_OPERATORS(X, T, N)                                       \
    X(T, N, div, /, /=) X(T, N, rem, %, %=)                             \
    X(T, N, shl, <<, <<=) X(T, N, shr, >>, >>=)

/* the binary operators whose right operand an object in memory may be,
   the instruction's own operand on x86-64, backend.md 6.1: a name and
   the operator */
#define MEMORY_OPERATORS(X, T, N)                                       \
    X(T, N, add, +) X(T, N, sub, -) X(T, N, mul, *) X(T, N, div, /)     \
    X(T, N, rem, %) X(T, N, and, &) X(T, N, or, |) X(T, N, xor, ^)      \
    X(T, N, lt, <) X(T, N, le, <=) X(T, N, gt, >) X(T, N, ge, >=)       \
    X(T, N, eq, ==) X(T, N, ne, !=)

/* a comparison of an object in memory with a constant, as a condition:
   a name, the operator, the constant */
#define KTESTS(X, T, N)                                                 \
    X(T, N, lt_5, <, 5) X(T, N, le_m1, <=, -1) X(T, N, gt_0, >, 0)      \
    X(T, N, ge_4096, >=, 4096) X(T, N, eq_0, ==, 0) X(T, N, ne_7, !=, 7)

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

/* The four rows beside the operand in memory, backend.md 6.1: an
   object read under a cast that keeps its bits, a comparison's value
   and a multiply by a constant.  Row b's pairs: the type an operation
   is in, the type of the object read under a cast to it, the letter
   of the object's globals and a name; an int under unsigned and back,
   a long under unsigned long and back, a pointer under long and under
   unsigned long. */
#define RECASTS(X)                                                      \
    X(unsigned, int, i, ui) X(int, unsigned, u, iu)                     \
    X(unsigned long, long, l, ull) X(long, unsigned long, ul, lul)      \
    X(long, char *, pc, lpc) X(unsigned long, int *, pi, ulpi)          \
    X(long, long *, pl, lpl)

/* the operators of MEMORY_OPERATORS and the six comparisons, each X
   given the arguments of the list's own first and then a name and the
   operator */
#define OPERATORS_OF(X, ...)                                            \
    X(__VA_ARGS__, add, +) X(__VA_ARGS__, sub, -)                       \
    X(__VA_ARGS__, mul, *) X(__VA_ARGS__, div, /)                       \
    X(__VA_ARGS__, rem, %) X(__VA_ARGS__, and, &)                       \
    X(__VA_ARGS__, or, |) X(__VA_ARGS__, xor, ^)                        \
    COMPARISONS_OF(X, __VA_ARGS__)

#define COMPARISONS_OF(X, ...)                                          \
    X(__VA_ARGS__, lt, <) X(__VA_ARGS__, le, <=)                        \
    X(__VA_ARGS__, gt, >) X(__VA_ARGS__, ge, >=)                        \
    X(__VA_ARGS__, eq, ==) X(__VA_ARGS__, ne, !=)
/* clang-format on */

/* the pointers row c compares with the null pointer: the type and the
   letter of its globals */
#define POINTERS(X) X(char *, pc) X(int *, pi) X(long *, pl)

/* the constant of x * c: a name and the constant; those an imul
   immediate holds, and one it does not, which keeps the load */
/* clang-format off */
#define FACTORS(X, T, N)                                                \
    X(T, N, 4, 4) X(T, N, m3, -3) X(T, N, 1000, 1000)                   \
    X(T, N, max, 0x7fffffff) X(T, N, min, -0x7fffffff - 1)              \
    X(T, N, big, 0x123456789)
/* clang-format on */
