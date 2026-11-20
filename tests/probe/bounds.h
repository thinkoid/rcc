/* The lists the bounds probe is generated from.  bounds.c makes
   functions of each entry, compiled by rcc and, as the reference, by
   clang -O0 under the prefix ref_.  bounds-main.c calls both builds
   with the object placed at the end of a mapped page whose next page
   is not mapped, and at the start of one whose previous page is not,
   so that an access wider than its object, or one that starts below
   it, faults (G10), and compares the results and the object's bytes.
   Each row reads or writes the object *p of a narrow type, through a
   pointer or as a member at a constant offset, *(T *) (q + 8): the
   rows that take an object in memory as their operand or load it. */

/* the narrow types and the letter each takes in a function's name; a
   long has nothing wider to be read as */
/* clang-format off */
#define TYPES(X)                                                        \
    X(char, c) X(signed char, sc) X(unsigned char, uc) X(short, s)      \
    X(unsigned short, us) X(int, i) X(unsigned, u)

/* rows whose value is an expression of the object O, *p or *(T *) (q +
   8), and an int y */
#define VALUES(X, T, N, O)                                              \
    X(T, N, read, O) X(T, N, widened, (long) O)                         \
    X(T, N, widened_unsigned, (unsigned long) O) X(T, N, not, !O)       \
    X(T, N, less, y < O) X(T, N, less_left, O < y)                      \
    X(T, N, equal, O == y) X(T, N, add, y + O) X(T, N, add_left, O + y) \
    X(T, N, sub, y - O) X(T, N, sub_left, O - y) X(T, N, mul, y * O)    \
    X(T, N, mul_left, O * y) X(T, N, and, y & O) X(T, N, and_left, O & y) \
    X(T, N, or, y | O) X(T, N, or_left, O | y) X(T, N, xor, y ^ O)      \
    X(T, N, xor_left, O ^ y) X(T, N, shl_left, O << (y & 7))            \
    X(T, N, shr_left, O >> (y & 7)) X(T, N, shl_count, y << (O & 7))    \
    X(T, N, div_left, O / y) X(T, N, rem_left, O % y)                   \
    X(T, N, product, O * 12) X(T, N, sum_long, (long) y + O)            \
    X(T, N, add_to, O += y) X(T, N, sub_from, O -= y)                   \
    X(T, N, mul_on, O *= y) X(T, N, and_to, O &= y)                     \
    X(T, N, or_to, O |= y) X(T, N, xor_to, O ^= y)                      \
    X(T, N, div_on, O /= y) X(T, N, rem_on, O %= y)                     \
    X(T, N, shl_on, O <<= (y & 7)) X(T, N, shr_on, O >>= (y & 7))       \
    X(T, N, times_on, O *= 12) X(T, N, add_constant, O += 3)            \
    X(T, N, pre_inc, ++O) X(T, N, post_inc, O++) X(T, N, pre_dec, --O)  \
    X(T, N, post_dec, O--) X(T, N, store_value, O = y)
/* clang-format on */

/* rows whose divisor is the object, which the main keeps from 0 */
#define DIVIDED(X, T, N, O) X(T, N, div, y / O) X(T, N, rem, y % O)

/* rows for their effect alone, the memory forms among them */
/* clang-format off */
#define STATEMENTS(X, T, N, O)                                          \
    X(T, N, add_statement, O += y) X(T, N, sub_statement, O -= y)       \
    X(T, N, and_statement, O &= y) X(T, N, or_statement, O |= y)        \
    X(T, N, xor_statement, O ^= y) X(T, N, mul_statement, O *= y)       \
    X(T, N, times_statement, O *= 12)                                   \
    X(T, N, constant_statement, O += 3) X(T, N, shl_statement, O <<= 3) \
    X(T, N, inc_statement, ++O) X(T, N, dec_statement, O--)             \
    X(T, N, store, O = y) X(T, N, store_constant, O = 5)

/* rows of a condition on the object */
#define CONDITIONS(X, T, N, O)                                          \
    X(T, N, test, O) X(T, N, test_not, !O) X(T, N, test_less, O < y)    \
    X(T, N, test_equal, O == y) X(T, N, test_and, O & y)                \
    X(T, N, test_add, (O += y) != 0)
/* clang-format on */

/* the object through p, and as a member at offset 8 through q */
#define POINTED(T) (*p)
#define MEMBER(T)  (*(T *) (q + 8))
