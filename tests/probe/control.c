/* The control probe: conditions as values and as jumps, the
   statements that branch, and values held across a branch, in rcc's C,
   for docs/aarch64.md chapter 7.  See control.h. */

#include "control.h"

/* docs/aarch64.md 6.3 and 7.1, per type: the comparisons, !, && and ||
   and ?: for their values; each as the condition of an if; the test of
   zero, also on the operand of a narrowing cast, whose upper bits are
   stale; and values held across the paths of a branch */
#define CONDITIONS(T, N)                                                \
    int lt_##N(T a, T b) { return a < b; }                              \
    int le_##N(T a, T b) { return a <= b; }                             \
    int gt_##N(T a, T b) { return a > b; }                              \
    int ge_##N(T a, T b) { return a >= b; }                             \
    int eq_##N(T a, T b) { return a == b; }                             \
    int ne_##N(T a, T b) { return a != b; }                             \
    int not_##N(T a) { return !a; }                                     \
    int notnot_##N(T a) { return !!a; }                                 \
    int notlt_##N(T a, T b) { return !(a < b); }                        \
    int and_##N(T a, T b) { return a && b; }                            \
    int or_##N(T a, T b) { return a || b; }                             \
    T sel_##N(T a, T b) { return a ? b : a; }                           \
    T min_##N(T a, T b) { return a < b ? a : b; }                       \
    int iflt_##N(T a, T b) { if (a < b) return 1; return 2; }           \
    int ifle_##N(T a, T b) { if (a <= b) return 1; return 2; }          \
    int ifgt_##N(T a, T b) { if (a > b) return 1; return 2; }           \
    int ifge_##N(T a, T b) { if (a >= b) return 1; return 2; }          \
    int ifeq_##N(T a, T b) { if (a == b) return 1; return 2; }          \
    int ifne_##N(T a, T b) { if (a != b) return 1; else return 2; }     \
    int truth_##N(T a) { if (a) return 1; else return 0; }              \
    int false_##N(T a) { if (!a) return 1; return 0; }                  \
    int both_##N(T a, T b) { if (a && b) return 1; return 0; }          \
    int either_##N(T a, T b) { if (a || b) return 1; return 0; }        \
    int neither_##N(T a, T b) { if (!(a || b)) return 1; return 0; }    \
    int stale_##N(long a) { if ((T) a) return 1; return 0; }            \
    int stale_not_##N(long a, long b) { return !(T) (a + b); }          \
    int stale_and_##N(long a, long b) { return (T) a && (T) (b + 1); }  \
    int mixed_##N(T a, T b)                                             \
    {                                                                   \
        T x = a;                                                        \
        int r = 0;                                                      \
        if (a < b && (x += b) || a == b)                                \
            r = 1;                                                      \
        return r + (x != a) * 2;                                        \
    }                                                                   \
    T held_##N(T a, T b)                                                \
    {                                                                   \
        return a + (a < b) * 2 + (b && a) * 4 + (a ? b : a) + (a || b)  \
               + (b ? a < b ? 1 : 2 : a ? 3 : 4);                       \
    }                                                                   \
    T arms_##N(T a, T b)                                                \
    {                                                                   \
        T x = 0;                                                        \
        T y = 0;                                                        \
        T r;                                                            \
        a ? (x = b) : (y = b);                                          \
        (void) (b && (x += 1));                                         \
        a || (y -= 1);                                                  \
        r = a < b ? x++ : y--;                                          \
        return r + x + y;                                               \
    }

TYPES(CONDITIONS)

/* a comparison with a constant, as a value and as a condition */
#define KCMP(T, N, name, op, constant)                                  \
    int k##N##_##name(T a) { return a op constant; }                    \
    int j##N##_##name(T a) { if (a op constant) return 1; return 0; }
#define KCMP_ALL(T, N) KCMPS(KCMP, T, N)

WIDE(KCMP_ALL)

/* the arms of a conditional in two types: the common type is long */
long arms_mixed(int c, int i, long l)
{
    return c ? i : l;
}

unsigned long arms_unsigned(int c, unsigned u, long l)
{
    return c ? u : l;
}

/* a condition that is a constant is still a test, docs/backend.md 7.1 */
int constant_conditions(int a)
{
    int r = 0;

    if (0)
        r += 1;
    if (1)
        r += 2;
    while (0)
        r += 4;
    if (a, 0)
        r += 8;

    return r + (1 ? a : 2) + (0 || a) + (1 && a);
}

/* while, docs/aarch64.md 7.2 */
int count(int n)
{
    int i = 0;
    int s = 0;

    while (i < n) {
        s += i;
        i++;
    }

    return s;
}

/* for, with a declaration and with an expression; an empty body */
int sum(int n)
{
    int s = 0;

    for (int i = 0; i < n; i++)
        s += i;

    return s;
}

int last(int n)
{
    int i;

    for (i = 0; i < n; i++)
        ;

    return i;
}

/* the comma in a for's initializer and step, and as a condition */
int pairs(int n)
{
    int i;
    int j;
    int s = 0;

    for (i = 0, j = 0; i < n; i++, j += 2)
        s += j;

    while (i--, i > 0)
        s -= 1;

    return s + i;
}

/* loops in loops, and an if and else inside */
long nested(int n, int m)
{
    long s = 0;

    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            if ((i ^ j) & 1)
                s += i * j;
            else
                s -= j;

    return s;
}

/* a loop whose condition is a value, and a return from inside it */
int gcd(int a, int b)
{
    while (b) {
        int t = a % b;
        a = b;
        b = t;
    }

    return a;
}

unsigned collatz(unsigned n)
{
    unsigned steps = 0;

    while (n != 1) {
        if (n & 1)
            n = 3 * n + 1;
        else
            n = n / 2;
        steps++;
    }

    return steps;
}

int first_bit(unsigned x)
{
    int i = 0;

    while (1) {
        if (x & 1)
            return i;
        if (i == 31)
            return 32;
        x >>= 1;
        i++;
    }
}

/* conditions of several operators, in a loop */
int primes(int n)
{
    int count = 0;

    for (int i = 2; i < n; i++) {
        int prime = 1;

        for (int d = 2; d * d <= i && prime; d++)
            if (i % d == 0)
                prime = 0;

        if (prime || i == 2)
            count++;
    }

    return count;
}

/* eighteen values held when the paths of a conditional come: all are
   saved before the first jump, docs/backend.md 6.7 */
int deep(int a, int b, int c, int d, int e, int f, int g, int h)
{
    return a - (b - (c - (d - (e - (f - (g - (h - (a - (b - (c - (d - (e -
           (f - (g - (h - (a - (b - (c ? d && e : f || g))))))))))))))))));
}

/* the same in long, the branch in the middle of the held values */
long deep_long(long a, long b, long c, long d, long e, long f, long g, long h)
{
    return a + (b * (c + (d * (e + (f * (g + (h * (a < b ? c : d)
           + (e && f)) - (g || h)) + (a ? b : c)) - !d) + (e == f)) - g) + h);
}
