/* The lists the jumps probe is generated from.  jumps.c makes functions
   of each entry, compiled by rcc and, as the reference, by clang -O0
   under the prefix ref_; jumps-main.c calls both over a table of values
   and compares.  do, break, continue, goto and labels, and an if whose
   body is one jump, docs/backend.md 7.2 and docs/aarch64.md 7.2.

   The integer types and the comparisons with a constant are
   control.h's, so a do's condition takes every form a while's takes
   there. */

#include "control.h"

/* an if whose body is one jump: each row, in each loop, on each
   condition, is the function if_<row>_<loop>_<condition>.  The rows
   are break, continue, goto and a return with no value, each with and
   without an else; both of the first two inside a switch in the loop;
   and break alone in a block.  The conditions are a comparison, a &&,
   a ! and a plain value. */
/* clang-format off */
#define IF_JUMP_CONDITIONS(X, row, loop)                                \
    X(row, loop, cmp) X(row, loop, land) X(row, loop, lnot)             \
    X(row, loop, value)

#define IF_JUMP_LOOPS(X, row)                                           \
    IF_JUMP_CONDITIONS(X, row, while) IF_JUMP_CONDITIONS(X, row, do)    \
    IF_JUMP_CONDITIONS(X, row, for)

#define IF_JUMPS(X)                                                     \
    IF_JUMP_LOOPS(X, break) IF_JUMP_LOOPS(X, break_else)                \
    IF_JUMP_LOOPS(X, continue) IF_JUMP_LOOPS(X, continue_else)          \
    IF_JUMP_LOOPS(X, goto) IF_JUMP_LOOPS(X, goto_else)                  \
    IF_JUMP_LOOPS(X, return) IF_JUMP_LOOPS(X, return_else)              \
    IF_JUMP_LOOPS(X, switch) IF_JUMP_LOOPS(X, braced)
/* clang-format on */
