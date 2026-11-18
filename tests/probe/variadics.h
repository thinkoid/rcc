/* The lists the variadics probe is generated from, lists.h's types,
   counts of named parameters and arguments.  variadics.c makes
   variadic functions in rcc's C, compiled by rcc and, as the
   reference, by clang -O0 under the prefix ref_; variadics-lib.c a
   function that takes a list, compiled by clang alone; and
   variadics-main.c calls each function of both builds with the same
   arguments and compares.  Each definition's va_start, 7.16.1.4, builds
   its list from the arguments the caller passed, in registers and on
   the stack, and the list is read by va_arg, copied, started again,
   and handed to the C library and to clang's code.  docs/shape.md
   chapter 5. */

#include <stdarg.h>
#include <stddef.h>

#include "lists.h"
