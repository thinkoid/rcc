/* The lists the globals probe is generated from.  globals.c makes
   functions of each entry, compiled by rcc and, as the reference, by
   clang -O0 under the prefix ref_.  globals-lib.c defines the objects
   globals.c only declares, and the functions it calls, compiled by
   clang alone and linked under both names.  globals-main.c calls both
   builds over a table of values and compares the results and the bytes
   of every object.  An object at file scope, docs/backend.md 6.3 and
   chapter 10 and docs/aarch64.md 6.3 and chapter 10: one the file
   defines, reached PC-relative, and one it only declares, reached
   through the GOT, read, written, stepped, subscripted and its address
   taken; the data of an initialized object and of a tentative one.

   The integer types are calls.h's. */

#include "calls.h"

/* the two kinds of object of each type, by the letter its names start
   with: d, which globals.c defines, and x, which it only declares and
   globals-lib.c defines */
#define KINDS(X, T, N) X(T, N, d) X(T, N, x)

/* the objects of a type and a kind: K_N, an array Ka_N[8], an array of
   arrays Km_N[3][4], and a pointer Kp_N; each array has its
   neighbours' alignment, so an access too wide lands in the next one */
#define ARRAY   8
#define ROWS    3
#define COLUMNS 4

/* initialized objects: a name, its type and its initializer, each
   entry a statement of its own; the constant converted to the object's
   type as an initializer is, which the main computes again for each */
#define INITIALIZED(X)                            \
    X(char, i_c, 'x');                            \
    X(signed char, i_sc, -100);                   \
    X(unsigned char, i_uc, 200);                  \
    X(short, i_s, -30000);                        \
    X(unsigned short, i_us, 60000);               \
    X(int, i_i, -123456789);                      \
    X(unsigned, i_u, 4000000000u);                \
    X(long, i_l, -1234567890123L);                \
    X(unsigned long, i_ul, 0xfedcba9876543210ul); \
    X(char, i_wrap, 300);                         \
    X(unsigned char, i_m1, -1);                   \
    X(unsigned short, i_trunc, 0x12345);          \
    X(int, i_expr, 40 + 2 * 3);                   \
    X(long, i_sum, 0x7fffffff + 1u);              \
    X(int, i_zero, 0);                            \
    X(unsigned long, i_ones, ~0ul);               \
    X(short, i_size, sizeof(long));
