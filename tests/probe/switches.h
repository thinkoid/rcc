/* The lists the switches probe is generated from.  switches.c makes
   functions of each entry, compiled by rcc and, as the reference, by
   clang -O0 under the prefix ref_; switches-main.c calls both over a
   table of values and compares.  The switch statement, docs/backend.md
   7.3 and docs/aarch64.md 7.3. */

#include "control.h"

/* the four types a switch dispatches in, the promoted ones, with the
   letter each takes in a function's name and its least and greatest
   values */
#define LIMITS(X)                                                 \
    X(int, i, (-2147483647 - 1), 2147483647)                      \
    X(unsigned, u, 0u, 4294967295u)                               \
    X(long, l, (-9223372036854775807L - 1), 9223372036854775807L) \
    X(unsigned long, ul, 0ul, 18446744073709551615ul)

/* the types narrower than int, promoted to it, 6.8.5.3p5 */
#define NARROW(X) \
    X(char, c)    \
    X(signed char, sc) X(unsigned char, uc) X(short, s) X(unsigned short, us)
