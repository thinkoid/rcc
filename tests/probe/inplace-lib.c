/* The functions inplace.c calls, compiled by clang -O0 alone.  The
   object is linked twice, as it is for rcc's build of inplace.c and
   under the prefix ref_ for clang's. */

#include "inplace.h"

/* The low four bits of the frame address, which are zero when the
   caller kept the stack pointer a multiple of 16 at the call.  Added
   to every result, so a caller that did not shows as a difference. */
#define SKEW                                                            \
    ((long) ((unsigned long) __builtin_frame_address(0) & 15) * 1000003L)

/* a call */
long lid(long a) { return a + SKEW; }
