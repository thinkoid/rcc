/* The lists the indirect probe is generated from.  indirect.c makes
   functions of each entry, compiled by rcc and, as the reference, by
   clang -O0 under the prefix ref_.  indirect-lib.c holds the functions
   they call through pointers, compiled by clang alone and linked under
   both names.  indirect-main.c calls both builds over a table of values
   and compares.  A function's address, and the call through a pointer
   to a function, in the three pairings of the calls probe,
   docs/backend.md chapters 8 and 10.

   The integer types, the parameter lists of one type and the sums that
   tell the positions apart are calls.h's. */

#include "calls.h"

/* pointers to functions of nought to twelve long parameters */
typedef long (*fn0_t)(void);
typedef long (*fn1_t)(long);
typedef long (*fn2_t)(long, long);
typedef long (*fn3_t)(long, long, long);
typedef long (*fn4_t)(long, long, long, long);
typedef long (*fn5_t)(long, long, long, long, long);
typedef long (*fn6_t)(long, long, long, long, long, long);
typedef long (*fn7_t)(long, long, long, long, long, long, long);
typedef long (*fn8_t)(P8(long));
typedef long (*fn9_t)(P9(long));
typedef long (*fn10_t)(P10(long));
typedef long (*fn11_t)(P11(long));
typedef long (*fn12_t)(P12(long));
typedef long (*mixed12_t)(MIXED12);
typedef long (*vmix_t)(int, ...);
typedef void (*note_t)(long);

/* pointers to functions of each type: a result and a parameter of the
   type, a result of the type from a long, and ten parameters of the
   type */
#define POINTERS(T, N)              \
    typedef T (*inc_##N##_t)(T);    \
    typedef T (*ret_##N##_t)(long); \
    typedef long (*tmix10_##N##_t)(P10(T));

TYPES(POINTERS)
