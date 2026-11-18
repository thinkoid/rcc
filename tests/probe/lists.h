/* The lists the lists probe is generated from.  lists.c makes rcc's
   functions that take a va_list and read it with va_arg, compiled by
   rcc and, as the reference, by clang -O0 under the prefix ref_;
   lists-lib.c makes the variadic functions, compiled by clang alone,
   whose va_start builds each list and hands it to one of them; and
   lists-main.c calls each through both builds and compares.  A list is
   7.16's, the target's: an array of one record on x86-64, which a
   parameter takes as the record's address, and a record on aarch64,
   passed as the address of the caller's copy.  docs/shape.md chapter
   5. */

#include <stdarg.h>

/* the types an argument is read at: a letter, for the names, and the
   type; char and short read the low bytes of the promoted argument */
#define TYPES(X)                                                        \
    X(i, int) X(u, unsigned) X(l, long) X(m, unsigned long)             \
    X(p, char *) X(c, char) X(s, short)

/* the counts of named parameters before the list's, which move the
   first argument it reads from register to register and past the
   registers, six on x86-64 and eight on aarch64; NAMED_k declares k
   parameters and NAMED_ARGS_k passes them */
#define COUNTS(X, A)                                                    \
    X(0, A) X(1, A) X(2, A) X(3, A) X(4, A) X(5, A) X(6, A) X(7, A)     \
    X(8, A)

#define NAMED_0
#define NAMED_1 long a1,
#define NAMED_2 long a1, long a2,
#define NAMED_3 long a1, long a2, long a3,
#define NAMED_4 long a1, long a2, long a3, long a4,
#define NAMED_5 long a1, long a2, long a3, long a4, long a5,
#define NAMED_6 long a1, long a2, long a3, long a4, long a5, long a6,
#define NAMED_7                                                         \
    long a1, long a2, long a3, long a4, long a5, long a6, long a7,
#define NAMED_8                                                         \
    long a1, long a2, long a3, long a4, long a5, long a6, long a7,      \
        long a8,

#define NAMED_ARGS_0
#define NAMED_ARGS_1 1,
#define NAMED_ARGS_2 1, 2,
#define NAMED_ARGS_3 1, 2, 3,
#define NAMED_ARGS_4 1, 2, 3, 4,
#define NAMED_ARGS_5 1, 2, 3, 4, 5,
#define NAMED_ARGS_6 1, 2, 3, 4, 5, 6,
#define NAMED_ARGS_7 1, 2, 3, 4, 5, 6, 7,
#define NAMED_ARGS_8 1, 2, 3, 4, 5, 6, 7, 8,

/* the most arguments a call passes after its count */
#define MOST 20
