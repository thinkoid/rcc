/* The functions registers.c calls, compiled by clang -O0 alone.  The
   object is linked twice, as it is for rcc's build of registers.c and
   under the prefix ref_ for clang's. */

#include "registers.h"

/* The low four bits of the frame address, which are zero when the
   caller kept the stack pointer a multiple of 16 at the call.  Added
   to every result, so a caller that did not shows as a difference. */
#define SKEW                                                            \
    ((long) ((unsigned long) __builtin_frame_address(0) & 15) * 1000003L)

/* a call, and twelve long arguments, each position by its own weight */
long lid(long a) { return a + SKEW; }
long lmix12(P12(long)) { return S12 + SKEW; }

/* a result of the type, which clang may return with stale bits above
   its width, since the convention has the caller extend it; and a long
   behind registers.c's declaration of the type, stale to 64 bits */
#define RESULTS(T, N)                                                   \
    T lret_##N(long x) { return x; }                                    \
    long lwide_##N(long x) { return x + SKEW; }

TYPES(RESULTS)
