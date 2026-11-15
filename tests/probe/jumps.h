/* The lists the jumps probe is generated from.  jumps.c makes functions
   of each entry, compiled by rcc and, as the reference, by clang -O0
   under the prefix ref_; jumps-main.c calls both over a table of values
   and compares.  do, break, continue, goto and labels, docs/backend.md
   7.2 and docs/aarch64.md 7.2.

   The integer types and the comparisons with a constant are
   control.h's, so a do's condition takes every form a while's takes
   there. */

#include "control.h"
