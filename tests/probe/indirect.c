/* The indirect probe: a function's address and the call through a
   pointer to a function, in rcc's C, for docs/backend.md chapters 8 and
   10 and docs/aarch64.md chapters 8 and 10.  Three pairings: the
   functions here called by clang's main through the pointers they
   return; the functions here calling clang's, of indirect-lib.c and of
   the main, through pointers; and the functions here calling each other
   through pointers.  See indirect.h. */

#include "indirect.h"

/* indirect-lib.c */
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
fn1_t lib_mix1(void);
long vmix(int n, ...);
long vmix_i(int n, ...);
void note(long v);
long noted(void);
long tick(long v);
int positive(long x);

#define LIBRARY(T, N)      \
        T ret_##N(long x); \
        long tmix10_##N(P10(T));

TYPES(LIBRARY)

/* functions of this file, called through pointers by its own code and
   by the main's */
long
twice(long a)
{
        return a * 2;
}

long
add2(long a, long b)
{
        return a + b * 3;
}

#define CALLEE(T, N)           \
        T inc_##N(T a)         \
        {                      \
                return a + 1;  \
        }                      \
        long par10_##N(P10(T)) \
        {                      \
                return S10;    \
        }

TYPES(CALLEE)

/* The address of a function, backend.md chapter 10: of one this file
   defines, PC-relative, and of one it only declares, from the GOT.  The
   main compares each with its own, and the declared one is compared
   here with the address the library's own code takes. */
fn1_t
defined_address(void)
{
        return twice;
}

fn1_t
declared_address(void)
{
        return &mix1;
}

int
declared_same(void)
{
        return (mix1 == lib_mix1()) + 2 * (&mix1 == lib_mix1());
}

int
defined_same(void)
{
        fn1_t p = twice;

        return (p == &twice) + 2 * (p == *twice) + 4 * (p != 0);
}

/* the address as a value: stored, compared with null and with another,
   a condition, an arm of ?:, and f, &f and *f alike */
int
null_tests(fn1_t p)
{
        return (p == 0) + 2 * (p != (void *) 0) + 4 * !p + 8 * (p ? 1 : 0) +
               16 * (0 == p);
}

int
address_forms(int k)
{
        fn1_t p = k ? twice : 0;
        fn1_t q = k ? &twice : mix1;

        if (!p)
                return q == mix1;

        return (p == q) + 2 * (*p == &twice) +
               4 * (p == (fn1_t) (fn0_t) twice) + 8 * (q != mix1);
}

/* Calls through a local, a parameter, a register variable, an array
   element, a call's result, a conditional and a cast, backend.md
   chapter 8; p(x), (*p)(x) and (**p)(x) are one call. */
long
through_local(long a)
{
        fn1_t p = mix1;

        return p(a) + (*p)(a + 1) + (**p)(a + 2);
}

long
through_parameter(fn1_t f, long a)
{
        return f(a) + (*f)(f(a));
}

long
through_register(long a, long b)
{
        register fn2_t p = mix2;
        register fn1_t q = twice;

        return p(a, b) + q(p(b, a)) + (*q)(a);
}

long
through_array(int k, long a)
{
        fn1_t t[3];

        t[0] = mix1;
        t[1] = twice;
        t[2] = tick;

        return t[k](a) + t[2 - k](a + 1) + noted();
}

fn1_t
pick(int k)
{
        return k ? twice : mix1;
}

long
through_call(int k, long a)
{
        return pick(k)(a) + pick(!k)(a);
}

long
through_conditional(int k, long a, long b)
{
        return (k ? mix2 : add2)(a, b) + (k ? &add2 : &mix2)(b, a);
}

long
through_cast(long a, long b)
{
        fn0_t p = (fn0_t) mix2;
        fn2_t q = (fn2_t) p;

        return q(a, b) + ((fn1_t) (fn0_t) twice)(a);
}

/* (*f)(x) and (&f)(x) on a function's name: the address formed and the
   call through it; f(x) and (f)(x), the direct call */
long
through_name(long a)
{
        return (*twice)(a) + (&twice)(a + 1) + (*mix1)(a) + (&mix1)(a) +
               twice(a) + (twice) (a);
}

/* recursion through a pointer to the function being defined */
long
ifact(long n)
{
        fn1_t self = ifact;

        return n < 2 ? 1 : n * self(n - 1);
}

fn1_t
ifact_address(void)
{
        return ifact;
}

/* nought to twelve arguments through a pointer to clang's functions */
long
icall0(void)
{
        fn0_t p = mix0;

        return p();
}

long
icall1(long a)
{
        fn1_t p = mix1;

        return p(a);
}

long
icall2(long a, long b)
{
        fn2_t p = mix2;

        return p(a, b);
}

long
icall3(long a, long b, long c)
{
        fn3_t p = mix3;

        return p(a, b, c);
}

long
icall4(long a, long b, long c, long d)
{
        fn4_t p = mix4;

        return p(a, b, c, d);
}

long
icall5(long a, long b, long c, long d, long e)
{
        fn5_t p = mix5;

        return p(a, b, c, d, e);
}

long
icall6(long a, long b, long c, long d, long e, long f)
{
        fn6_t p = mix6;

        return p(a, b, c, d, e, f);
}

long
icall7(long a, long b, long c, long d, long e, long f, long g)
{
        fn7_t p = mix7;

        return p(a, b, c, d, e, f, g);
}

long
icall8(P8(long))
{
        fn8_t p = mix8;

        return p(a, b, c, d, e, f, g, h);
}

long
icall9(P8(long))
{
        fn9_t p = mix9;

        return p(a, b, c, d, e, f, g, h, a - b);
}

long
icall10(P8(long))
{
        fn10_t p = mix10;

        return p(a, b, c, d, e, f, g, h, a - b, c * d);
}

long
icall11(P8(long))
{
        fn11_t p = mix11;

        return p(a, b, c, d, e, f, g, h, a - b, c * d, e ^ f);
}

long
icall12(P8(long))
{
        fn12_t p = mix12;

        return p(a, b, c, d, e, f, g, h, a - b, c * d, e ^ f, g + h);
}

long
icall_mixed(long a, long b)
{
        mixed12_t p = mixed12;

        return p(a, b, a, b, a, b, a, b, a, b, a, b) +
               (*p)(b, a + 1, b - a, a, b, a, b, a, b, a, b, a);
}

/* A result and parameters of each type through a pointer: to rcc's own
   functions, which the main also calls through the pointer pick_inc
   returns; to clang's, of the library and of the main; with ten
   arguments, past the registers.  A narrow result is extended by the
   caller, as for a named call. */
#define NARROWS(T, N)                                               \
        inc_##N##_t pick_inc_##N(void)                              \
        {                                                           \
                return inc_##N;                                     \
        }                                                           \
        long ichain_##N(T a)                                        \
        {                                                           \
                inc_##N##_t p = inc_##N;                            \
                                                                    \
                return p(a) + p(p(a)) + (*p)(a);                    \
        }                                                           \
        long iret_##N(long x)                                       \
        {                                                           \
                ret_##N##_t p = ret_##N;                            \
                T t = p(x);                                         \
                                                                    \
                return t + p(x + 1) + (p(x) < p(x + 1));            \
        }                                                           \
        long itmix10_##N(T a, T b)                                  \
        {                                                           \
                tmix10_##N##_t p = tmix10_##N;                      \
                                                                    \
                return p(a, b, a - b, b, a, a * b, a, b, a, b + 1); \
        }                                                           \
        long ipar10_##N(T a, T b)                                   \
        {                                                           \
                tmix10_##N##_t p = par10_##N;                       \
                                                                    \
                return p(b, a, b, a, b, a, b, a, inc_##N(a), b);    \
        }                                                           \
        long iapply_##N(inc_##N##_t f, T a)                         \
        {                                                           \
                return f(a) + f(f(a));                              \
        }

TYPES(NARROWS)

/* a variable number of arguments through a pointer: passed as any
   others are, the narrow ones promoted to int */
long
ivariadic(int a, long b, char c, short d)
{
        vmix_t v = vmix;
        vmix_t vi = vmix_i;

        return v(0) + v(1, b) + (*v)(3, b, (long) a, b) +
               v(11, b, b + 1, b + 2, b + 3, b + 4, b + 5, b + 6, b + 7, b + 8,
                 b + 9, (long) d) +
               vi(4, a, c, d, c + d) + vi(9, a, c, d, a, c, d, a, c, d);
}

/* calls through a pointer for their effect, to a function of void */
long
ieffects(int a, int b)
{
        note_t n = note;
        fn1_t t = tick;

        n(a);
        (*n)(b);
        a < b ? t(1) : t(2);
        t(a);
        (void) t(b);

        return noted();
}

/* The callee before the arguments, G13: an argument that writes the
   pointer the callee read leaves the call to the function read, and a
   callee that is a call is made before the arguments. */
long
callee_first(long a)
{
        fn1_t p = mix1;
        long r = p((p = twice, a));

        return r + p(a);
}

fn2_t
noting(long k)
{
        note(k + 100);

        return mix2;
}

long
callee_order(long a, long b)
{
        long r = noting(a)(tick(a), tick(b));

        return r + noted();
}

/* values held when a call through a pointer comes, and calls through
   pointers in its arguments */
long
iheld(long a, long b, long c)
{
        fn1_t p = mix1;
        fn2_t q = mix2;

        return a + p(b) - c * q(a, b) + (p(a) - (*p)(b)) * q(c, a);
}

long
inest(long a, long b, long c)
{
        fn1_t p = mix1;
        fn3_t r = mix3;

        return r(p(a), mix2(b, p(c)), r(a, p(b), b));
}

/* eighteen values held when the callee is read */
long
ideep(P8(long))
{
        fn2_t p = mix2;

        return a -
               (b -
                (c -
                 (d -
                  (e -
                   (f -
                    (g -
                     (h -
                      (a -
                       (b -
                        (c -
                         (d -
                          (e -
                           (f - (g - (h - (a - (b - p(g, h))))))))))))))))));
}

/* a call through a pointer as a condition and on the paths of a
   branch */
int
icondition(long a, long b)
{
        int (*p)(long) = positive;

        if (p(a) && (*p)(b))
                return 1;

        return p(a + b) ? 2 : 3;
}

long
ipaths(long a, long b)
{
        fn1_t p = mix1;
        fn2_t q = mix2;

        return a + (a < b ? p(b) : q(a, b)) + b;
}

/* a callee slot the load's offset does not reach, beside a frame of
   40000 bytes */
long
far_icall(long a, char b, short c)
{
        char pad[40000];
        mixed12_t p = mixed12;
        fn1_t q = mix1;
        long x = q(a);

        return p(b, c, a, x, b, c, a, x, b, c, b, a) + x;
}
