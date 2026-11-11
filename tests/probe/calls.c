/* The calls probe: calls, in rcc's C, for docs/aarch64.md chapter 8
   and the parameters of 5.2.  Three pairings: the functions here
   called by clang's main; the functions here calling clang's, of
   calls-lib.c; and the functions here calling each other.  See
   calls.h. */

#include "calls.h"

/* calls-lib.c */
long mix0(void);
long mix1(long a);
long mix2(long a, long b);
long mix3(long a, long b, long c);
long mix4(long a, long b, long c, long d);
long mix5(long a, long b, long c, long d, long e);
long mix6(long a, long b, long c, long d, long e, long f);
long mix7(long a, long b, long c, long d, long e, long f, long g);
long mix8(P8(long));
long mix9(P9(long));
long mix10(P10(long));
long mix11(P11(long));
long mix12(P12(long));
long mixed12(MIXED12);
long vmix(int n, ...);
long vmix_i(int n, ...);
void note(long v);
long noted(void);
long tick(long v);
int positive(long x);

#define LIBRARY(T, N)                                                   \
    long tmix9_##N(P9(T));                                              \
    long tmix10_##N(P10(T));                                            \
    T ret_##N(long x);                                                  \
    T wide_##N(long x);

TYPES(LIBRARY)

/* The callee's side, docs/aarch64.md 5.2 and 6.3: a parameter in a
   register and one past the registers, read at its type's width,
   whatever the caller left above it; a stack parameter written; a
   narrow result, which the callee does not extend. */
#define CALLEE(T, N)                                                    \
    long par1_##N(T a) { return a; }                                    \
    long par8_##N(P8(T)) { return S8; }                                 \
    long par9_##N(P9(T)) { return S9; }                                 \
    long par10_##N(P10(T)) { return S10; }                              \
    long step10_##N(P10(T))                                             \
    {                                                                   \
        i += a;                                                         \
        j++;                                                            \
        h = i;                                                          \
        return S10;                                                     \
    }                                                                   \
    T res_##N(long x) { return x; }                                     \
    T inc_##N(T a) { return a + 1; }

TYPES(CALLEE)

long par12(MIXED12)
{
    return S12;
}

/* stack parameters above a frame the load's offset does not reach */
long far_param(long a, long b, long c, long d, long e, long f, long g,
               long h, char i, short j, long k, int l)
{
    char pad[40000];

    i += 1;

    return S12;
}

/* The caller's side, chapter 8: nought to twelve arguments to clang's
   functions. */
long call0(void) { return mix0(); }
long call1(long a) { return mix1(a); }
long call2(long a, long b) { return mix2(a, b); }
long call3(long a, long b, long c) { return mix3(a, b, c); }
long call4(long a, long b, long c, long d) { return mix4(a, b, c, d); }
long call5(long a, long b, long c, long d, long e) { return mix5(a, b, c, d, e); }
long call6(long a, long b, long c, long d, long e, long f) { return mix6(a, b, c, d, e, f); }
long call7(long a, long b, long c, long d, long e, long f, long g) { return mix7(a, b, c, d, e, f, g); }
long call8(P8(long)) { return mix8(a, b, c, d, e, f, g, h); }
long call9(P8(long)) { return mix9(a, b, c, d, e, f, g, h, a - b); }
long call10(P8(long)) { return mix10(a, b, c, d, e, f, g, h, a - b, c * d); }
long call11(P8(long)) { return mix11(a, b, c, d, e, f, g, h, a - b, c * d, e ^ f); }
long call12(P8(long)) { return mix12(a, b, c, d, e, f, g, h, a - b, c * d, e ^ f, g + h); }

/* arguments that are constants, the store row of 6.6 */
long call_constants(long a)
{
    return mix12(0, 1, -1, 4096, 0x12345, a, 0, -5000, 0x123456789abcdef0, 0, 7, a)
           + mixed12(0, -2, 300000, a, 255, 65535, 0, 1, -128, 0, 65, -1);
}

/* arguments of every type, a parameter passed on and an expression,
   which is converted to the parameter's type */
#define CALLER(T, N)                                                    \
    long tcall9_##N(T a, T b)                                           \
    {                                                                   \
        return tmix9_##N(a, b, a, b, a, b, a, b, a + b);                \
    }                                                                   \
    long tcall10_##N(T a, T b)                                          \
    {                                                                   \
        return tmix10_##N(a, b, a - b, b, a, a * b, a, b, a, b + 1);    \
    }

TYPES(CALLER)

long call_mixed(long a, long b)
{
    return mixed12(a, b, a, b, a, b, a, b, a, b, a, b)
           + mixed12(b, a + 1, b - a, a, b, a, b, a, b, a, b, a);
}

/* A result in each place a value goes, 6.3: the caller reads it valid
   to its type's width and no further.  F is ret, an honest function of
   the type, and wide, a long behind a declaration of the type. */
#define RESULT(F, T, N)                                                 \
    long F##_long_##N(long x) { return F##_##N(x); }                    \
    int F##_plus_##N(long x) { return F##_##N(x) + 1; }                 \
    int F##_test_##N(long x) { if (F##_##N(x)) return 1; return 0; }    \
    int F##_not_##N(long x) { return !F##_##N(x); }                     \
    int F##_less_##N(long x, long y) { return F##_##N(x) < F##_##N(y); } \
    T F##_return_##N(long x) { return F##_##N(x); }                     \
    long F##_store_##N(long x)                                          \
    {                                                                   \
        T t = F##_##N(x);                                               \
        long l;                                                         \
        l = F##_##N(x + 1);                                             \
        t += F##_##N(l);                                                \
        return t + l;                                                   \
    }                                                                   \
    long F##_pass_##N(long x)                                           \
    {                                                                   \
        return tmix9_##N(F##_##N(x), 1, 2, 3, 4, 5, 6, 7, F##_##N(x + 1)) \
               + mix1(F##_##N(x));                                      \
    }                                                                   \
    long F##_paths_##N(long x, long y)                                  \
    {                                                                   \
        return (x < y ? F##_##N(x) : F##_##N(y))                        \
               + (F##_##N(x) && F##_##N(y)) * 2;                        \
    }

#define RESULT_RET(T, N) RESULT(ret, T, N)
#define RESULT_WIDE(T, N) RESULT(wide, T, N)

TYPES(RESULT_RET)
TYPES(RESULT_WIDE)

/* a variable number of arguments: they are passed as any others are,
   the narrow ones promoted to int */
long variadic(int a, long b, char c, short d)
{
    return vmix(0) + vmix(1, b) + vmix(3, b, (long) a, b)
           + vmix(11, b, b + 1, b + 2, b + 3, b + 4, b + 5, b + 6, b + 7,
                  b + 8, b + 9, (long) d)
           + vmix_i(4, a, c, d, c + d)
           + vmix_i(9, a, c, d, a, c, d, a, c, d);
}

/* a call for its effect, a result dropped, and a function of void */
long effects(int a, int b)
{
    note(a);
    note(b);
    a < b ? tick(1) : tick(2);
    mix1(a);
    (void) mix2(a, b);

    for (int k = 0; k < 3; note(k), k++)
        ;

    return noted();
}

/* The arguments left to right, G13.  The standard leaves the order
   open; clang's is the same. */
long order(long a, long b)
{
    long r = mix3(tick(a), tick(b), tick(a + b));

    return r + noted();
}

long order10(long a)
{
    long r = mix10(tick(a), tick(2), tick(3), tick(4), tick(5), tick(6),
                   tick(7), tick(8), tick(9), tick(10));

    return r + noted();
}

/* values held when a call comes, and calls in the arguments of calls */
long held(long a, long b, long c)
{
    return a + mix1(b) - c * mix2(a, b) + (mix1(a) - mix1(b)) * mix3(a, b, c);
}

long nest(long a, long b, long c)
{
    return mix3(mix1(a), mix2(b, mix1(c)), mix3(a, mix1(b), mix2(c, a)));
}

long nest10(long a, long b)
{
    return mix10(a, mix9(b, a, b, a, b, a, b, a, b), b, a, b, a, b, a,
                 mix10(a, b, a, b, a, b, a, b, a, mix1(b)), mix1(b) - a);
}

/* eighteen values held when the call comes */
long deep_call(P8(long))
{
    return a - (b - (c - (d - (e - (f - (g - (h - (a - (b - (c - (d - (e -
           (f - (g - (h - (a - (b - mix2(g, h))))))))))))))))));
}

/* an argument that needs every register, and a call inside it */
long deep_argument(P8(long))
{
    return mix2(a - (b - (c - (d - (e - (f - (g - (h - (a - (b - (c - (d -
                (e - (f - (g - (h - (a - (b - mix1(c)))))))))))))))))), h);
}

/* a call as a condition, and calls on the paths of a branch with a
   value held across them */
int condition(long a, long b)
{
    if (positive(a) && positive(b))
        return 1;

    if (positive(a) || !positive(b))
        return 2;

    return positive(a + b) ? 3 : 4;
}

long paths(long a, long b)
{
    return a + (positive(a) ? mix1(b) : mix2(a, b))
           + (positive(b) && positive(a)) + b;
}

int loop(int n)
{
    int s = 0;

    for (int k = 0; positive(n - k); k++)
        s += mix1(k);

    while (positive(n))
        n = n - 1 - positive(n);

    return s + n;
}

/* a call from a frame whose slots the load's offset does not reach */
long far_call(long a, char b, short c)
{
    char pad[40000];
    long x = mix1(a);

    return mixed12(b, c, a, x, b, c, a, x, b, c, b, a)
           + par12(b, c, a, x, b, c, a, x, b, c, b, a) + x;
}

/* The third pairing: rcc's functions calling rcc's.  inc_<n> returns a
   narrow result unextended and its caller extends it. */
#define CHAIN(T, N)                                                     \
    long chain_##N(T a) { return inc_##N(a); }                          \
    long chain_twice_##N(T a) { return inc_##N(inc_##N(a)) + inc_##N(a); } \
    int chain_test_##N(T a) { return inc_##N(a) ? 1 : 2; }              \
    long chain10_##N(T a, T b)                                          \
    {                                                                   \
        return par10_##N(a, b, a, b, a, b, a, b, inc_##N(a), inc_##N(b)) \
               + par9_##N(b, a, b, a, b, a, b, a, b)                    \
               + step10_##N(a, b, a, b, a, b, a, b, a, b);              \
    }

TYPES(CHAIN)

/* recursion */
int fact(int n)
{
    return n < 2 ? 1 : n * fact(n - 1);
}

int fib(int n)
{
    if (n < 2)
        return n;

    return fib(n - 1) + fib(n - 2);
}

int odd(int n);

int even(int n)
{
    return n == 0 ? 1 : odd(n - 1);
}

int odd(int n)
{
    return n == 0 ? 0 : even(n - 1);
}

int ack(int m, int n)
{
    if (m == 0)
        return n + 1;

    if (n == 0)
        return ack(m - 1, 1);

    return ack(m - 1, ack(m, n - 1));
}

/* ten parameters turned through the registers and the stack */
long spin(int n, P9(long))
{
    if (n == 0)
        return a - b * 2 + c * 3 - d * 4 + e * 5 - f * 6 + g * 7 - h * 8 + i * 9;

    return spin(n - 1, b, c, d, e, f, g, h, i, a + n);
}
