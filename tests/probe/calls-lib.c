/* The functions calls.c calls, compiled by clang -O0 alone.  The
   object is linked twice, as it is for rcc's build of calls.c and
   under the prefix ref_ for clang's, so each build has its own copy
   of the state below. */

#include <stdarg.h>

#include "calls.h"

/* The low four bits of the frame address, which are zero when the
   caller kept the stack pointer a multiple of 16 at the call.  Added
   to every result, so a caller that did not shows as a difference. */
#define SKEW ((long) ((unsigned long) __builtin_frame_address(0) & 15) * 1000003L)

/* nought to twelve long arguments, each position by its own weight */
long mix0(void) { return 17 + SKEW; }
long mix1(long a) { return S1 + SKEW; }
long mix2(long a, long b) { return S2 + SKEW; }
long mix3(long a, long b, long c) { return S3 + SKEW; }
long mix4(long a, long b, long c, long d) { return S4 + SKEW; }
long mix5(long a, long b, long c, long d, long e) { return S5 + SKEW; }
long mix6(long a, long b, long c, long d, long e, long f) { return S6 + SKEW; }
long mix7(long a, long b, long c, long d, long e, long f, long g) { return S7 + SKEW; }
long mix8(P8(long)) { return S8 + SKEW; }
long mix9(P9(long)) { return S9 + SKEW; }
long mix10(P10(long)) { return S10 + SKEW; }
long mix11(P11(long)) { return S11 + SKEW; }
long mix12(P12(long)) { return S12 + SKEW; }

/* nine and ten arguments of one type; a result of the type, which
   clang returns with stale bits above its width, since the convention
   has the caller extend it; and a long behind a declaration of the
   type, stale to 64 bits */
#define TYPED(T, N)                                                     \
    long tmix9_##N(P9(T)) { return S9 + SKEW; }                         \
    long tmix10_##N(P10(T)) { return S10 + SKEW; }                      \
    T ret_##N(long x) { return x; }                                     \
    long wide_##N(long x) { return x; }

TYPES(TYPED)

long mixed12(MIXED12) { return S12 + SKEW; }

/* a variable number of longs, and of ints */
long vmix(int n, ...)
{
    va_list ap;
    long sum = n + SKEW;

    va_start(ap, n);
    for (int k = 0; k < n; ++k)
        sum += va_arg(ap, long) * (2 * k + 3);
    va_end(ap);

    return sum;
}

long vmix_i(int n, ...)
{
    va_list ap;
    long sum = n + SKEW;

    va_start(ap, n);
    for (int k = 0; k < n; ++k)
        sum += (long) va_arg(ap, int) * (2 * k + 3);
    va_end(ap);

    return sum;
}

/* a call for its effect: note() folds its argument into the state in
   the order of the calls, noted() returns the state and clears it */
static long state;

void note(long v)
{
    state = state * 31 + v;
}

long noted(void)
{
    const long was = state + SKEW;

    state = 0;

    return was;
}

long tick(long v)
{
    note(v);

    return v;
}

int positive(long x)
{
    return x > 0;
}
