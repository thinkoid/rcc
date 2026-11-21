/* The generic probe: a generic selection, 6.5.2.1, in rcc's C, one
   function of each of generic.h's selections, returning the number it
   selects.  See generic.h. */

#include "generic.h"

#define SELECT(N, E, ...)                        \
        int select_##N(void)                     \
        {                                        \
                return _Generic(E, __VA_ARGS__); \
        }

SELECTIONS(SELECT)
