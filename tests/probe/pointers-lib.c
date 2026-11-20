/* The functions pointers.c calls, compiled by clang -O0 alone.  The
   object is linked twice, as it is for rcc's build of pointers.c and
   under the prefix ref_ for clang's. */

#include "pointers.h"

/* The low four bits of the frame address, which are zero when the
   caller kept the stack pointer a multiple of 16 at the call.  Added
   to every result, so a caller that did not shows as a difference. */
#define SKEW \
        ((long) ((unsigned long) __builtin_frame_address(0) & 15) * 1000003L)

/* a long behind pointers.c's declaration of the type: the bits above
   the type's width are the caller's to ignore */
#define WIDE_RESULT(T, N)        \
        long wide_##N(long x)    \
        {                        \
                return x + SKEW; \
        }

TYPES(WIDE_RESULT)

long
id(long a)
{
        return a + SKEW;
}

/* the objects pointers.c only declares: a pointer of each pointee of
   POINTER_TYPES and an index of each type of LONG_INDEXES */
#define POINTER_TYPE(E, T, R) typedef T(*ptr_##E) R;

POINTER_TYPES(POINTER_TYPE)

#define POINTER_DECLARED(E, T, R) ptr_##E q_##E##_declared;

POINTER_TYPES(POINTER_DECLARED)

long n_l_declared;
unsigned long n_ul_declared;
