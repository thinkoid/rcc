/* The registers probe: register variables as places, in rcc's C, for
   docs/aarch64.md chapter 4 and backend.md chapters 4 and 6.7.  A
   register variable initialized, read by every operator, assigned and
   tested; its reads held across the paths of a branch and across a
   call; the callee-saved registers given back; register parameters;
   blocks; pointers; a large frame; depth; and G5's unwritten narrow
   variable.  No compound assignment and no ++ or -- of a register
   variable: a loop steps by i = i + 1.  A function keeps to five
   register variables live at once, x86-64's count, where a sixth
   would take a slot there; the functions that have more say so.  See
   registers.h. */

#include "registers.h"

/* registers-lib.c */
long lid(long a);
long lmix12(P12(long));

#define LIBRARY(T, N)                                                   \
    T lret_##N(long x);                                                 \
    T lwide_##N(long x);

TYPES(LIBRARY)

/* rcc's own callees, in rcc's build: a long, and a narrow result,
   which the callee does not extend */
long twice(long v)
{
    return v * 2 + 1;
}

#define OWN(T, N) T own_##N(long x) { return x; }

TYPES(OWN)

/* Initialized from a parameter, from an expression, from another
   register variable, from a constant: 0, small, one no mov encodes on
   aarch64, negative, 64-bit, each converted to the type; declared
   without an initializer and assigned later.  Each is read back wide
   with its own weight. */
#define INIT(T, N)                                                      \
    long init_##N(T a, T b)                                             \
    {                                                                   \
        register T p = a;                                               \
        register T e = a * 3 - b;                                       \
        register T q = p;                                               \
        register T d;                                                   \
        register T z = 0;                                               \
        d = b;                                                          \
        return p + e * 3L + q * 5L + d * 7L + z * 11L;                  \
    }                                                                   \
    long kinit_##N(void)                                                \
    {                                                                   \
        register T z = 0;                                               \
        register T k = 5;                                               \
        register T w = 0x12345;                                         \
        register T n = -5;                                              \
        register T h = 0x123456789abcdef0;                              \
        return z + k * 3L + w * 5L + n * 7L + h * 11L;                  \
    }                                                                   \
    long kinit2_##N(void)                                               \
    {                                                                   \
        register T m = -0x12345;                                        \
        register T f = 0xfff000;                                        \
        register T g = -1;                                              \
        register T v = 0x80000000;                                      \
        register T u = 0x7fffffff00000080;                              \
        return m + f * 3L + g * 5L + v * 7L + u * 11L;                  \
    }

TYPES(INIT)

/* A read as the left and as the right operand, the other a register
   variable, a slot, a constant the instruction encodes and one it does
   not; every value then reads both variables again, so an instruction
   that wrote its result into a variable's register shows. */
#define BINARY(T, N, name, op, small, large)                            \
    long name##_##N(T a, T b)                                           \
    {                                                                   \
        register T x = a;                                               \
        register T y = b;                                               \
        T s = b;                                                        \
        long r = (x op y) + x * 3L + y * 5L;                            \
        r = r * 7 + (y op x) + x * 3L + y * 5L;                         \
        r = r * 7 + (x op s) + x * 3L;                                  \
        r = r * 7 + (s op x) + x * 3L;                                  \
        r = r * 7 + (x op small) + x * 3L;                              \
        r = r * 7 + (small op x) + x * 3L;                              \
        r = r * 7 + (x op large) + x * 3L;                              \
        r = r * 7 + (large op x) + x * 3L;                              \
        return r * 7 + x * 3L + y * 5L;                                 \
    }

/* a shift by a register count, of a register, a slot and a constant */
#define SHIFT(T, N, name, op)                                           \
    long name##_##N(T a, T n)                                           \
    {                                                                   \
        register T x = a;                                               \
        register T c = n;                                               \
        T s = a;                                                        \
        T t = n;                                                        \
        long r = (x op c) + x * 3L + c * 5L;                            \
        r = r * 7 + (x op t) + x * 3L;                                  \
        r = r * 7 + (s op c) + c * 3L;                                  \
        r = r * 7 + (x op 1) + x * 3L;                                  \
        r = r * 7 + (x op 31) + x * 3L;                                 \
        r = r * 7 + (5 op c) + c * 3L;                                  \
        r = r * 7 + (0x12345 op c) + c * 3L;                            \
        return r * 7 + x * 3L + c * 5L;                                 \
    }

/* unary -, ~ and !, each value followed by a read of the variable */
#define UNARY(T, N)                                                     \
    long unary_##N(T a)                                                 \
    {                                                                   \
        register T x = a;                                               \
        long r = -x + x * 3L;                                           \
        r = r * 7 + ~x + x * 3L;                                        \
        r = r * 7 + !x + x * 3L;                                        \
        r = r * 7 + !!x + (-x) * 5L + ~x;                               \
        return r * 7 + x;                                               \
    }

#define BINARY_ALL(T, N)                                                \
    OPERATORS(BINARY, T, N) DIVISIONS(BINARY, T, N) SHIFTS(SHIFT, T, N) \
    UNARY(T, N)

TYPES(BINARY_ALL)

/* every cast of a register variable to a wider and a narrower type,
   the variable read again after */
#define CAST(F, FN, T, TN)                                              \
    long cast_##FN##_##TN(F a)                                          \
    {                                                                   \
        register F x = a;                                               \
        return (long) (T) x * 3 + x;                                    \
    }
#define CASTS(F, FN) TYPES2(CAST, F, FN)

TYPES(CASTS)

/* Plain assignment, as a statement and for its value, from a constant,
   a slot, another register variable, a call's result; a value wider
   than the type, its truncation read back wide. */
#define ASSIGN(T, N)                                                    \
    long assign_##N(T a, T b, long w)                                   \
    {                                                                   \
        register T x;                                                   \
        register T y = a;                                               \
        T s = b;                                                        \
        long r;                                                         \
        x = 5;                                                          \
        r = x;                                                          \
        x = 0x12345;                                                    \
        r = r * 7 + x;                                                  \
        x = -0x123456789;                                               \
        r = r * 7 + x;                                                  \
        x = s;                                                          \
        r = r * 7 + x;                                                  \
        x = y;                                                          \
        r = r * 7 + x + y * 3L;                                         \
        x = lid(w);                                                     \
        r = r * 7 + x;                                                  \
        x = twice(w);                                                   \
        r = r * 7 + x;                                                  \
        x = w;                                                          \
        r = r * 7 + x;                                                  \
        x = w * 3 + 1;                                                  \
        r = r * 7 + x;                                                  \
        y = x = w - 3;                                                  \
        r = r * 7 + x + y * 3L;                                         \
        r = r * 7 + (x = b) + x;                                        \
        r = r * 7 + (long) (x = w) + x;                                 \
        r = r * 7 + (y = x = w + 7) * 5L + x + y;                       \
        return r * 7 + x + y * 3L;                                      \
    }                                                                   \
    long assign_order_##N(T a, T b)                                     \
    {                                                                   \
        register T x = a;                                               \
        return x + (x = b) + x * 3L;                                    \
    }

TYPES(ASSIGN)

/* A register variable as the condition of if, while and for, and as
   the operand of &&, ||, ?: and !; compared with a register, a slot
   and a constant; a narrow one tested for zero.  k & (k - 1) clears
   the lowest bit set, so each loop runs once per bit of the type. */
#define CONDITION(T, N)                                                 \
    long cond_##N(T a, T b)                                             \
    {                                                                   \
        register T x = a;                                               \
        register T y = b;                                               \
        register T k;                                                   \
        T s = b;                                                        \
        long r = 0;                                                     \
        if (x)                                                          \
            r = r + 1;                                                  \
        if (!x)                                                         \
            r = r + 2;                                                  \
        if (x < y)                                                      \
            r = r + 4;                                                  \
        if (x == s)                                                     \
            r = r + 8;                                                  \
        if (x >= 4095)                                                  \
            r = r + 16;                                                 \
        if (x != 0x12345)                                               \
            r = r + 32;                                                 \
        if (x && y)                                                     \
            r = r + 64;                                                 \
        if (x || s)                                                     \
            r = r + 128;                                                \
        if (y > x ? x : y)                                              \
            r = r + 256;                                                \
        r = r * 7 + (x ? y : s) + (x && y) * 3 + (x || y) * 5 + !x * 9  \
            + (x > y ? 1 : 2) * 11 + (s <= x) * 13 + (x == 0) * 17;     \
        k = x;                                                          \
        while (k) {                                                     \
            k = k & (k - 1);                                            \
            r = r + 1000;                                               \
        }                                                               \
        for (k = y; k; k = k & (k - 1))                                 \
            r = r + 100000;                                             \
        for (register int i = 0; i < x && i < 5; i = i + 1)             \
            r = r + 10000000;                                           \
        return r + x + y * 3L;                                          \
    }

TYPES(CONDITION)

/* Held values across the paths of &&, || and ?:, backend.md 6.7: a
   read no path writes stays in its register; a read an arm writes is
   saved before the first divergence; nested.  Each read is the
   immediate left operand of the branch, so it is held, unconsumed,
   where the paths diverge; each write is of a variable whose read is
   held.  See the main on the order. */
#define HELD(T, N)                                                      \
    long held_##N(T a, T b, int c)                                      \
    {                                                                   \
        register T x = a;                                               \
        register T y = b;                                               \
        long r = x + (c ? 1 : 2);                                       \
        r = r * 7 + (x + (c ? y : 2)) + y;                              \
        r = r * 7 + (x + (c ? (x = 9) : 2));                            \
        x = a;                                                          \
        r = r * 7 + (x + (c && (x = 9)));                               \
        x = a;                                                          \
        r = r * 7 + (x + (c || (x = 0)));                               \
        x = a;                                                          \
        r = r * 7 + (x + (y + (c ? (b ? (x = 9) : y) : (y = 11))));     \
        r = r * 7 + x + y;                                              \
        x = a;                                                          \
        y = b;                                                          \
        r = r * 7 + (x + (y + (c ? x : y ? (x = y) : 7)));              \
        r = r * 7 + (x + (y + (c && y || (x = 3) && (y = 5))));         \
        return r * 7 + x + y * 3L;                                      \
    }

TYPES(HELD)

/* Calls: register variables live across a call to clang's function
   and to rcc's; a register variable as an argument, as each of
   twelve; a call's narrow result into a narrow and a wide register
   variable, by the initializer and by the assignment; x + f(x), and
   x + f(x = 3), on the order. */
#define CALLS(T, N)                                                     \
    long across_##N(T a, T b)                                           \
    {                                                                   \
        register T x = a;                                               \
        register T y = b;                                               \
        long r = lid(b);                                                \
        r = r * 7 + x + y * 3L;                                         \
        r = r * 7 + twice(a);                                           \
        r = r * 7 + x + y * 3L;                                         \
        r = r * 7 + lmix12(x, y, x, y, x, y, x, y, x, y, x, y);         \
        r = r * 7 + lmix12(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, x);       \
        r = r * 7 + lmix12(y, 2, 3, 4, 5, 6, x, 8, y, 10, 11, 12);      \
        r = r * 7 + (x + lid(x));                                       \
        r = r * 7 + (x + twice(x)) + y;                                 \
        return r * 7 + x + y * 3L;                                      \
    }                                                                   \
    long result_##N(long w)                                             \
    {                                                                   \
        register T n = lret_##N(w);                                     \
        register long l = lret_##N(w + 1);                              \
        register T m = lwide_##N(w);                                    \
        register long k = lwide_##N(w - 1);                             \
        register T o = own_##N(w + 2);                                  \
        long r = n + l * 3 + m * 5 + k * 7 + o * 11;                    \
        n = lwide_##N(w + 3);                                           \
        l = lwide_##N(w + 4);                                           \
        m = own_##N(w + 5);                                             \
        k = own_##N(w + 6);                                             \
        return r * 7 + n + l * 3 + m * 5 + k * 7 + o * 11;              \
    }                                                                   \
    long call_order_##N(T a)                                            \
    {                                                                   \
        register T x = a;                                               \
        return x + lid(x = 3) + x * 5L;                                 \
    }                                                                   \
    long own_order_##N(T a)                                             \
    {                                                                   \
        register T x = a;                                               \
        return x + twice(x = 3) + x * 5L;                               \
    }

TYPES(CALLS)

/* recursion with live register variables, the parameters register too */
long rsum(register int n, register long acc)
{
    register long m = n * 3;

    if (n <= 0)
        return acc;

    return rsum(n - 1, acc + m) + m - n;
}

int rfib(register int n)
{
    register int a;

    if (n < 2)
        return n;

    a = rfib(n - 1);

    return a + rfib(n - 2);
}

/* The callee-saved registers: clobber<k> declares and writes k
   register variables, the first k of either target's set; keep<k> has
   ten live, of distinct values, across the call to it, and sums them
   with distinct weights.  On x86-64 the sixth to the tenth take slots,
   with the warning.  The save area of aarch64 stores pairs and an odd
   last one, so k runs from 1 to 10. */
long clobber1(long v)
{
    register long a = v + 1;
    return a;
}

long clobber2(long v)
{
    register long a = v + 1;
    register long b = v * 3;
    return a + b;
}

long clobber3(long v)
{
    register long a = v + 1;
    register long b = v * 3;
    register long c = v ^ 0x55;
    return a + b + c;
}

long clobber4(long v)
{
    register long a = v + 1;
    register long b = v * 3;
    register long c = v ^ 0x55;
    register long d = ~v;
    return a + b + c + d;
}

long clobber5(long v)
{
    register long a = v + 1;
    register long b = v * 3;
    register long c = v ^ 0x55;
    register long d = ~v;
    register long e = v - 0x12345;
    return a + b + c + d + e;
}

long clobber6(long v)
{
    register long a = v + 1;
    register long b = v * 3;
    register long c = v ^ 0x55;
    register long d = ~v;
    register long e = v - 0x12345;
    register long f = v * 5;
    return a + b + c + d + e + f;
}

long clobber7(long v)
{
    register long a = v + 1;
    register long b = v * 3;
    register long c = v ^ 0x55;
    register long d = ~v;
    register long e = v - 0x12345;
    register long f = v * 5;
    register long g = v + 77;
    return a + b + c + d + e + f + g;
}

long clobber8(long v)
{
    register long a = v + 1;
    register long b = v * 3;
    register long c = v ^ 0x55;
    register long d = ~v;
    register long e = v - 0x12345;
    register long f = v * 5;
    register long g = v + 77;
    register long h = v << 3;
    return a + b + c + d + e + f + g + h;
}

long clobber9(long v)
{
    register long a = v + 1;
    register long b = v * 3;
    register long c = v ^ 0x55;
    register long d = ~v;
    register long e = v - 0x12345;
    register long f = v * 5;
    register long g = v + 77;
    register long h = v << 3;
    register long i = -v;
    return a + b + c + d + e + f + g + h + i;
}

long clobber10(long v)
{
    register long a = v + 1;
    register long b = v * 3;
    register long c = v ^ 0x55;
    register long d = ~v;
    register long e = v - 0x12345;
    register long f = v * 5;
    register long g = v + 77;
    register long h = v << 3;
    register long i = -v;
    register long j = v * 7 + 1;
    return a + b + c + d + e + f + g + h + i + j;
}

#define KEEP(k)                                                         \
    long keep##k(long v)                                                \
    {                                                                   \
        register long r0 = v + 11;                                      \
        register long r1 = v * 3 - 7;                                   \
        register long r2 = v ^ 0x5a5a;                                  \
        register long r3 = ~v;                                          \
        register long r4 = v * 17;                                      \
        register long r5 = v - 1000;                                    \
        register long r6 = v * 5 + 2;                                   \
        register long r7 = v ^ -1000;                                   \
        register long r8 = v * 9;                                       \
        register long r9 = 12345 - v;                                   \
        long c = clobber##k(v * 5 + 1);                                 \
        return r0 * 3 + r1 * 5 + r2 * 7 + r3 * 11 + r4 * 13 + r5 * 17  \
               + r6 * 19 + r7 * 23 + r8 * 29 + r9 * 31 + c;             \
    }

KEEP(1)
KEEP(2)
KEEP(3)
KEEP(4)
KEEP(5)
KEEP(6)
KEEP(7)
KEEP(8)
KEEP(9)
KEEP(10)

/* Register parameters of each type, in each position: the first five,
   the sixth to the tenth, the eleventh and twelfth mixed with ordinary
   ones, all twelve; one alone.  The main passes whole longs, so a
   narrow one's upper bits are whatever the caller left, which the
   callee must not read.  Each is read twice, widened from its type
   and promoted first, since the first reads the type's bits alone and
   the second the whole of what the callee keeps.  The rcc pairing
   calls them with the type's values. */
#define PARAMETERS(T, N)                                                \
    long rpar1_##N(register T a) { return a; }                          \
    long rfirst_##N(PFIRST(T)) { return (S12) + N12 * 43; }             \
    long rmiddle_##N(PMIDDLE(T)) { return (S12) + N12 * 43; }           \
    long rlast_##N(PLAST(T)) { return (S12) + N12 * 43; }               \
    long rall_##N(PALL(T)) { return (S12) + N12 * 43; }                 \
    long rwritten_##N(register T a, T b, register T c)                  \
    {                                                                   \
        long r = a + c * 3L;                                            \
        a = b;                                                          \
        c = a + c;                                                      \
        return r * 7 + a + c * 3L + b * 5L;                             \
    }                                                                   \
    long rcall_##N(T a, T b)                                            \
    {                                                                   \
        return rpar1_##N(a)                                             \
               + rfirst_##N(a, b, a, b, a, b, a, b, a, b, a + b, a - b) \
               + rmiddle_##N(b, a, b, a, b, a, b, a, b, a, b * 3, a)    \
               + rlast_##N(a, b, a + 1, b, a, b, a, b, a, b, a, b)      \
               + rall_##N(a, a, b, b, a, a, b, b, a, a, b, b)           \
               + rwritten_##N(a, b, b - a);                             \
    }

TYPES(PARAMETERS)

/* Blocks: sibling blocks whose variables share registers, an inner
   block's with the outer's still live, and eleven live at once, more
   than either target has, so the eleventh takes a slot. */
long blocks(long a, long b)
{
    register long x = a;
    long r = 0;

    {
        register long y = b;
        register long z = a ^ b;
        r = y * 3 + z;
    }

    {
        register int u = a;
        register char c = b;
        register unsigned short h = a - b;
        r = r * 7 + u + c * 5 + h * 9 + x;
    }

    {
        register long p = a + 1;
        {
            register long q = b + 2;
            r = r * 7 + p * 3 + q + x;
        }
        register signed char q = a;
        r = r * 7 + p * 3 + q + x;
    }

    return r * 7 + x;
}

long eleven(long a, long b)
{
    register long r0 = a;
    register long r1 = b;
    register long r2 = a + b;
    register long r3 = a - b;
    register long r4 = a ^ b;
    register long r5 = a * 3;
    register long r6 = b * 5;
    register long r7 = ~a;
    register long r8 = ~b;
    register long r9 = a * b;
    register long r10 = a | b;

    return r0 + r1 * 3 + r2 * 5 + r3 * 7 + r4 * 11 + r5 * 13 + r6 * 17
           + r7 * 19 + r8 * 23 + r9 * 29 + r10 * 31 + lid(r10);
}

/* Pointers: a register pointer the base of *p, p[i], p[3] and a store
   through it, the index a register int and a register long; p + n,
   p - n, p - q, comparison, &p[i] and &p[0] on register operands.  The
   main's buffer is the memory; a pointer result is an offset. */
#define POINTER(E, T)                                                   \
    long pread_##E(T *base, long i, int j)                              \
    {                                                                   \
        register T *p = base;                                           \
        register long k = i;                                            \
        register int m = j;                                             \
        long r = *p + p[k] * 3L + p[m] * 5L + p[3] * 7L + *(p + k) * 11L; \
        return r * 7 + p[k + m] + (p[0] == *p) + (long) (p - base);     \
    }                                                                   \
    void pwrite_##E(T *base, long i, int j, long v)                     \
    {                                                                   \
        register T *p = base;                                           \
        register long k = i;                                            \
        register int m = j;                                             \
        *p = v;                                                         \
        p[k] = v + 1;                                                   \
        p[m] = v + 2;                                                   \
        p[3] = v + 3;                                                   \
        *(p + k + m) = v + 4;                                           \
    }                                                                   \
    long parith_##E(T *base, long n, int m)                             \
    {                                                                   \
        register T *p = base;                                           \
        register T *q = base + 4;                                       \
        register long k = n;                                            \
        register int j = m;                                             \
        register T *z = 0;                                              \
        long r = (p + k) - q;                                           \
        r = r * 7 + ((p - k) - q);                                      \
        r = r * 7 + (q - p) + (p - q) * 3;                              \
        r = r * 7 + (p < q) + (p + k <= q) * 2 + (p + j > q) * 4        \
            + (p + k == q) * 8 + (p != q - j) * 16 + (z == 0) * 32      \
            + (q >= p) * 64;                                            \
        r = r * 7 + (&p[k] - p) + (&p[j] - q) * 3 + (&p[0] - p) * 5     \
            + (&p[0] == p) * 9;                                         \
        r = r * 7 + ((char *) (q - j) - (char *) base);                 \
        p = q + k;                                                      \
        return r * 7 + (p - base) + (q - base) * 3;                     \
    }

SCALARS(POINTER)

/* p = p + 1 walks a string */
long pwalk(char *s)
{
    register char *p = s;
    register long h = 0;

    while (*p) {
        h = h * 31 + *p;
        p = p + 1;
    }

    return h * 7 + (p - s);
}

long puwalk(unsigned char *s, int n)
{
    register unsigned char *p = s;
    register unsigned char *end = s + n;
    register long h = 0;

    for (; p < end; p = p + 1)
        h = h * 31 + *p;

    return h * 7 + (end - p);
}

/* Frames: register variables before and after an array of 40000
   bytes, a slot beyond the reach of a load beside the save area, and a
   call. */
long frame_far(long a, long b)
{
    register long x = a;
    register int y = b;
    char pad[40000];
    register long z = a - b;
    register char c = b;
    long s = a ^ b;

    pad[0] = c;
    pad[39999] = y;
    s = s + x + lid(z);

    return x * 3 + y * 5 + z * 7 + c * 11 + s + pad[0] + pad[39999] * 13;
}

/* Depth, backend.md 6.2: an expression of nineteen operands held at
   once, register reads among them, one narrow, so its extension needs
   a register too; then the same over a call. */
long deep_reg(P8(long))
{
    register long x = a;
    register long y = b;
    register short z = c;

    return x - (y - (z - (d - (e - (f - (g - (h - (x - (y - (z - (d - (e -
           (f - (g - (h - (x - (y - z)))))))))))))))));
}

long deep_reg_call(P8(long))
{
    register long x = a;
    register int y = b;
    register unsigned char z = c;

    return x * (y + (z * (d + (e * (f + (g * (h + (x * (y + (z * (d + (e *
           (f + (g * (h + (x * (y + lid(z))))))))))))))))));
}

/* G5, the unwritten narrow variable: fill<n> sets ten register longs to
   v, each in a register of the callee-saved set or a slot, and calls
   unwritten<n>, which declares ten register variables of the type with
   no initializer and returns facts that hold for any value of the
   type.  clang's build returns the same constants: a generator that
   did not extend the register at the declaration reads more bits than
   the type has. */
#define FACTS(c, T) ((c == (T) c) + ((long) c == (long) (T) c) * 2)

#define UNWRITTEN(T, N)                                                 \
    long unwritten_##N(void)                                            \
    {                                                                   \
        register T c0;                                                  \
        register T c1;                                                  \
        register T c2;                                                  \
        register T c3;                                                  \
        register T c4;                                                  \
        register T c5;                                                  \
        register T c6;                                                  \
        register T c7;                                                  \
        register T c8;                                                  \
        register T c9;                                                  \
        return FACTS(c0, T) + FACTS(c1, T) * 4 + FACTS(c2, T) * 16      \
               + FACTS(c3, T) * 64 + FACTS(c4, T) * 256                 \
               + FACTS(c5, T) * 1024 + FACTS(c6, T) * 4096              \
               + FACTS(c7, T) * 16384 + FACTS(c8, T) * 65536            \
               + FACTS(c9, T) * 262144;                                 \
    }                                                                   \
    long fill_##N(long v)                                               \
    {                                                                   \
        register long r0 = v;                                           \
        register long r1 = v;                                           \
        register long r2 = v;                                           \
        register long r3 = v;                                           \
        register long r4 = v;                                           \
        register long r5 = v;                                           \
        register long r6 = v;                                           \
        register long r7 = v;                                           \
        register long r8 = v;                                           \
        register long r9 = v;                                           \
        return unwritten_##N() + (r0 ^ r9);                             \
    }

CLAIMED(UNWRITTEN)
