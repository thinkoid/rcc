/* The lists the statics probe is generated from.  statics.c makes
   functions of each entry, compiled by rcc and, as the reference, by
   clang -O0 under the prefix ref_.  statics-lib.c defines a static
   object and a static function of names statics.c gives its own, and
   the functions that read them; it is compiled by clang alone and
   linked under both names.  statics-main.c calls both builds and
   compares the results with each other and with what they should be.

   static, docs/backend.md chapter 10 and docs/aarch64.md chapter 10:
   an object and a function of internal linkage, 6.2.2p3, and a block's
   object of static storage duration, 6.2.4p3, which keeps its value
   from call to call under a label of its own.  No other file reaches
   them by name, so the main reads and writes them through the
   functions of statics.c alone.

   The integer types are calls.h's. */

#include "calls.h"

/* the value each type's initialized objects take, an entry a type; the
   main converts it to the type itself */
#define INITIAL(X)               \
    X(char, c, 'x')              \
    X(signed char, sc, -100)     \
    X(unsigned char, uc, 200)    \
    X(short, s, -30000)          \
    X(unsigned short, us, 60000) \
    X(int, i, -123456789)        \
    X(unsigned, u, 4000000000u)  \
    X(long, l, -1234567890123L)  \
    X(unsigned long, ul, 0xfedcba9876543210ul)

/* the type of a pointer to the static function, one name for an
   object and its reference */
typedef long (*helper_t)(long);
