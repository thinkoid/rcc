/* The straight probe: every function is straight-line code on
   integers, in rcc's C, for the expressions of docs/aarch64.md
   chapter 6 and the frame of chapter 5.  See straight.h. */

#include "straight.h"

/* the operators of docs/aarch64.md 6.3, on two operands of one type */
#define OPERATORS(T, N)                                                 \
    T add_##N(T a, T b) { return a + b; }                               \
    T sub_##N(T a, T b) { return a - b; }                               \
    T mul_##N(T a, T b) { return a * b; }                               \
    T div_##N(T a, T b) { return a / b; }                               \
    T rem_##N(T a, T b) { return a % b; }                               \
    T and_##N(T a, T b) { return a & b; }                               \
    T or_##N(T a, T b) { return a | b; }                                \
    T xor_##N(T a, T b) { return a ^ b; }                               \
    T shl_##N(T a, int n) { return a << n; }                            \
    T shr_##N(T a, int n) { return a >> n; }                            \
    T neg_##N(T a) { return -a; }                                       \
    T not_##N(T a) { return ~a; }

TYPES(OPERATORS)

/* locals, and every assignment in the effect context: each statement's
   value is dropped */
#define LOCALS(T, N)                                                    \
    T loc_##N(T a, T b)                                                 \
    {                                                                   \
        T x = a;                                                        \
        T y;                                                            \
        T z = 0;                                                        \
        y = b;                                                          \
        z = 0;                                                          \
        z = 1;                                                          \
        x = x + y;                                                      \
        x += b;                                                         \
        x -= 1;                                                         \
        x *= 3;                                                         \
        x <<= 1;                                                        \
        x >>= 1;                                                        \
        x |= 1;                                                         \
        x ^= b;                                                         \
        x &= a | 5;                                                     \
        x /= 3;                                                         \
        y %= 7;                                                         \
        ++x;                                                            \
        x++;                                                            \
        --y;                                                            \
        y--;                                                            \
        z += x;                                                         \
        return x + y + z;                                               \
    }

TYPES(LOCALS)

/* the same assignments for their values; each statement writes each
   object once and reads it nowhere else, so the order of evaluation is
   not what is compared */
#define VALUES(T, N)                                                    \
    T val_##N(T a, T b)                                                 \
    {                                                                   \
        T x = a;                                                        \
        T y = b;                                                        \
        T r;                                                            \
        T s;                                                            \
        T t;                                                            \
        T u;                                                            \
        T v;                                                            \
        r = (x += b) + (y -= a);                                        \
        s = x++ + ++y;                                                  \
        t = y-- + --x;                                                  \
        u = (x = b) + (y *= 2);                                         \
        v = (x >>= 1) + (y ^= 85);                                      \
        return r + s + t + u + v + x + y;                               \
    }

TYPES(VALUES)

/* the cast matrix: every type to every type */
#define CAST(F, FN, T, TN)                                              \
    T cast_##FN##_##TN(F a) { return (T) a; }
#define CASTS(F, FN) TYPES2(CAST, F, FN)

TYPES(CASTS)

/* the width rule, docs/aarch64.md 6.3: a cast after an instruction
   that left the register valid to more, or to less */
long chain_char(int a) { return (long) (char) a; }
long chain_int(long a, long b) { return (long) (int) (a + b); }
unsigned long chain_unsigned(long a, long b) { return (unsigned long) (unsigned) (a + b); }
int chain_short(int a) { return (int) (short) (a + 1); }
long chain_uchar(int a) { return (long) (unsigned char) (a * 3); }
long chain_usum(unsigned a, unsigned b) { return (long) (a + b); }
long chain_sum(int a, int b) { return (long) (a + b); }
unsigned long chain_neg(int a) { return (unsigned long) -a; }
long chain_locals(int a)
{
    signed char c = a;
    short s = c;
    long l = s;
    unsigned char u = l + 200;
    unsigned w = u;
    return l + w;
}

/* an operator with a constant right operand */
#define KONST(T, N, name, op, constant)                                 \
    T k##N##_##name(T a) { return a op constant; }
#define KONST_ALL(T, N) KONSTS(KONST, T, N)
#define KONST_64(T, N) KONSTS64(KONST, T, N)

WIDE(KONST_ALL)
KONST_64(long, l)
KONST_64(unsigned long, ul)

/* a constant that must be held */
#define HOLD(name, T, value)                                            \
    T held_##name(void) { return value; }

HELD(HOLD)

/* eight parameters, the convention's whole register set, of mixed
   types */
long mix_sum(char a, short b, int c, long d,
             unsigned char e, unsigned short f, unsigned g, unsigned long h)
{
    return a + b + c + d + e + f + g + h;
}

long mix_ops(signed char a, short b, int c, long d,
             unsigned char e, unsigned short f, unsigned g, unsigned long h)
{
    return a - b * c + (d ^ e) - (f | g) + h / 3 + (a << 3) + (g >> 5);
}

/* eighteen values held at once: the fifteen expression registers run
   out, docs/aarch64.md 6.2 */
int spill(int a, int b, int c, int d, int e, int f, int g, int h)
{
    return a - (b - (c - (d - (e - (f - (g - (h - (a - (b - (c - (d - (e -
           (f - (g - (h - (a - (b - c)))))))))))))))));
}

long spill_long(long a, long b, long c, long d, long e, long f, long g, long h)
{
    return a * (b + (c * (d + (e * (f + (g * (h + (a * (b + (c * (d + (e *
           (f + (g * (h + (a * (b + c)))))))))))))))));
}

/* slots beyond the reach of the instruction, docs/aarch64.md 5.3, in a
   frame of more than 4096 bytes, 5.2: the arrays are declared and
   never used */
long far(int a, long b)
{
    int near = a;
    int pad1[9216];
    int x = a;
    char c = a;
    int pad2[9216];
    long y = b;
    short s = b;
    char pad3[3];
    unsigned char u = a;

    x = x + 1;
    y += x;
    c += 2;
    s -= c;
    ++u;
    near ^= u;

    return near + x + y + c + s + u;
}

/* a frame of exactly 4096 bytes and more, and one just under */
int frame_page(int a)
{
    char pad[4064];
    int x = a;

    return x + 1;
}

int frame_two_pages(int a)
{
    char pad[8192];
    int x = a;

    return x + 2;
}

/* an expression statement's value is dropped */
int unused(int a, int b)
{
    a + 1;
    a * b;
    (void) a;
    -a;

    return a - b;
}

/* main falls off its end elsewhere; here a function of no parameters */
int nothing(void)
{
    return 42;
}
