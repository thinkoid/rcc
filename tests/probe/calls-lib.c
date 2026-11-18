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

/* Calls that write the object their caller read on the left of the
   operator: put_<n> writes v into order_<n>, which calls.c defines,
   bump_<n> writes v + 1 into moved_<n>, which this file defines, and
   put_at_<n> writes v through the pointer it is given.  Each returns
   v. */
#define WRITERS(T, N)                                                   \
    extern T order_##N;                                                 \
    T moved_##N;                                                        \
    T put_##N(T v)                                                      \
    {                                                                   \
        order_##N = v;                                                  \
        return v;                                                       \
    }                                                                   \
    T bump_##N(T v)                                                     \
    {                                                                   \
        moved_##N = (T) (v + 1);                                        \
        return v;                                                       \
    }                                                                   \
    T put_at_##N(T *p, T v)                                             \
    {                                                                   \
        *p = v;                                                         \
        return v;                                                       \
    }

TYPES(WRITERS)

/* The many-argument calls' callees: n, then each argument by its
   position's weight, which an argument stored at another's place
   changes. */
#define MANY_ITEM(p) p,

#define MANY_CALLEE(N, BLOCK)                                           \
    long many_##N(MANY_PARAMETERS(N, BLOCK))                            \
    {                                                                   \
        const long v[] = { BLOCK(MANY_ITEM, a) W8(MANY_ITEM, b)         \
                               W8(MANY_ITEM, c) };                      \
        long sum = n + SKEW;                                            \
                                                                        \
        for (unsigned long k = 0; k < sizeof v / sizeof *v; ++k)        \
            sum += v[k] * (long) (2 * k + 3);                           \
                                                                        \
        return sum;                                                     \
    }

MANY_TYPES(MANY_CALLEE)
