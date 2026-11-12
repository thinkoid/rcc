/* The inplace probe: compound assignment and ++ and -- on a register
   variable, in place, in rcc's C, for docs/aarch64.md chapter 4 and
   backend.md chapters 4 and 6.7.  Every compound assignment on every
   type, as a statement and for its value; mixed widths; ++ and --; a
   register pointer stepped, walked, and the string idioms of the
   classic register style; loops; the order of reads and in-place
   writes; calls and blocks across in-place writes.  A function keeps
   to five register variables live at once, x86-64's count.  See
   inplace.h.

   A variable read back after a write is read at its promoted width
   too, (x + 1) * 3L, as registers.h's N12 reads a parameter.  A
   narrow one read only through a widening cast is extended from its
   own bits by the cast, so that read alone would not see what its
   register holds above them, where a write that skipped the
   re-extension leaves stale bits. */

#include "inplace.h"

/* inplace-lib.c */
long lid(long a);

/* rcc's own callee, in rcc's build */
long twice(long v)
{
    return v * 2 + 1;
}

/* Every compound assignment on a register variable, the right operand
   a constant the instruction encodes, one it does not, a slot, a
   register variable and a call to clang's function and to rcc's; as a
   statement, each followed by a read of the variable back wide, and
   for its value.  A narrow variable is driven past its range by the
   constants and re-extended. */
#define COMPOUND(T, N, name, op, small, large)                          \
    long name##_##N(T a, T b, long w)                                   \
    {                                                                   \
        register T x = a;                                               \
        register T y = b;                                               \
        T s = b;                                                        \
        long r;                                                         \
        x op small;                                                     \
        r = x + (x + 1) * 3L;                                           \
        x op large;                                                     \
        r = r * 7 + x + (x + 1) * 3L;                                   \
        x op s;                                                         \
        r = r * 7 + x + y + (x + 1) * 3L;                               \
        x op y;                                                         \
        r = r * 7 + x + y * 3L + (x + 1) * 5L;                          \
        x op lid(w);                                                    \
        r = r * 7 + x + (x + 1) * 3L;                                   \
        x op twice(w);                                                  \
        r = r * 7 + x + (x + 1) * 3L;                                   \
        r = r * 7 + (x op small) + (x + 1) * 3L;                        \
        r = r * 7 + (x op large) + x + (x + 1) * 3L;                    \
        r = r * 7 + (x op y) + x + (x + 1) * 3L;                        \
        r = r * 7 + (y op x) + y + (y + 1) * 3L;                        \
        r = r * 7 + (x op lid(w + 1)) + x + (x + 1) * 3L;               \
        r = r * 7 + (y op s) * 3L + y + (y + 1) * 5L;                   \
        return r * 7 + x + y * 3L;                                      \
    }

/* the divisions: the divisor never the variable divided, nor a value
   the main has not kept from 0 and -1; the call's is 2 to 65 */
#define CDIVISION(T, N, name, op, small, large)                         \
    long name##_##N(T a, T b, long w)                                   \
    {                                                                   \
        register T x = a;                                               \
        register T y = b;                                               \
        register T z = a;                                               \
        T s = b;                                                        \
        long r;                                                         \
        x op small;                                                     \
        r = x + (x + 1) * 3L;                                           \
        x = a;                                                          \
        x op large;                                                     \
        r = r * 7 + x + (x + 1) * 3L;                                   \
        x = a;                                                          \
        x op s;                                                         \
        r = r * 7 + x + y + (x + 1) * 3L;                               \
        x = a;                                                          \
        x op y;                                                         \
        r = r * 7 + x + y * 3L + (x + 1) * 5L;                          \
        x = a;                                                          \
        x op lid((w & 63) + 2);                                         \
        r = r * 7 + x + (x + 1) * 3L;                                   \
        r = r * 7 + (z op small) + z + (z + 1) * 3L;                    \
        z = a;                                                          \
        r = r * 7 + (z op y) + z + y + (z + 1) * 3L;                    \
        z = a;                                                          \
        r = r * 7 + (z op lid((w & 15) + 2)) + z + (z + 1) * 3L;        \
        return r * 7 + x + y * 3L + z * 5L;                             \
    }

/* the shifts: a constant count, a register one, a slot and a call's */
#define CSHIFT(T, N, name, op)                                          \
    long name##_##N(T a, T n, long w)                                   \
    {                                                                   \
        register T x = a;                                               \
        register T c = n;                                               \
        T t = n;                                                        \
        long r;                                                         \
        x op 1;                                                         \
        r = x + (x + 1) * 3L;                                           \
        x = a;                                                          \
        x op 31;                                                        \
        r = r * 7 + x + (x + 1) * 3L;                                   \
        x = a;                                                          \
        x op c;                                                         \
        r = r * 7 + x + c + (x + 1) * 3L;                               \
        x = a;                                                          \
        x op t;                                                         \
        r = r * 7 + x + (x + 1) * 3L;                                   \
        x = a;                                                          \
        x op lid(w & 31);                                               \
        r = r * 7 + x + (x + 1) * 3L;                                   \
        x = a;                                                          \
        r = r * 7 + (x op 3) + x + (x + 1) * 3L;                        \
        r = r * 7 + (x op c) + x + c + (x + 1) * 3L;                    \
        return r * 7 + x + c * 3L;                                      \
    }

/* ++ and --, prefix and postfix, as a statement and for the value; the
   values table puts a narrow one at each side of its limits */
#define STEP(T, N)                                                      \
    long step_##N(T a)                                                  \
    {                                                                   \
        register T x = a;                                               \
        long r;                                                         \
        x++;                                                            \
        r = x + (x + 1) * 3L;                                           \
        ++x;                                                            \
        r = r * 7 + x + (x + 1) * 3L;                                   \
        x--;                                                            \
        --x;                                                            \
        --x;                                                            \
        r = r * 7 + x + (x + 1) * 3L;                                   \
        r = r * 7 + x++;                                                \
        r = r * 7 + x + (x + 1) * 3L;                                   \
        r = r * 7 + ++x + (x + 1) * 3L;                                 \
        r = r * 7 + x--;                                                \
        r = r * 7 + x + (x + 1) * 3L;                                   \
        r = r * 7 + --x + (x + 1) * 3L;                                 \
        return r * 7 + x;                                               \
    }                                                                   \
    long steps_##N(T a, T b)                                            \
    {                                                                   \
        register T x = a;                                               \
        register T y = b;                                               \
        long r = x++ * 3L + y-- * 5L;                                   \
        r = r * 7 + ++x * 3L + --y * 5L;                                \
        r = r * 7 + (x++ < y--) + (++x == --y) * 2;                     \
        return r * 7 + x + y * 3L;                                      \
    }

#define TYPE(T, N)                                                      \
    COMPOUNDS(COMPOUND, T, N) DIVISIONS(CDIVISION, T, N)                \
    SHIFTS(CSHIFT, T, N) STEP(T, N)

TYPES(TYPE)

/* mixed widths: the operation at the common type, the result
   converted back and the register re-extended; as statements, and for
   the values */
long widths(int a, unsigned b, long w, char c, short d)
{
    register int i = a;
    register unsigned u = b;
    register char ch = c;
    register long l = w;
    register short sh = d;
    long r;

    i += w;
    r = i;
    u += w;
    r = r * 7 + u;
    ch *= a;
    r = r * 7 + ch + (ch + 1) * 3L;
    l <<= a & 31;
    r = r * 7 + l;
    l += a;
    r = r * 7 + l;
    sh /= w;
    r = r * 7 + sh + (sh + 1) * 3L;
    u -= w;
    r = r * 7 + u;
    i *= w;
    r = r * 7 + i;

    return r * 7 + i + u * 3L + ch * 5L + l * 7L + sh * 11L;
}

long widths_value(int a, unsigned b, long w, char c, short d)
{
    register int i = a;
    register unsigned u = b;
    register char ch = c;
    register long l = w;
    register short sh = d;
    long r;

    r = (i += w);
    r = r * 7 + (u += w);
    r = r * 7 + (ch *= a) + (ch + 1) * 3L;
    r = r * 7 + (l <<= a & 31);
    r = r * 7 + (l += a);
    r = r * 7 + (sh /= w) + (sh + 1) * 3L;
    r = r * 7 + (u -= w) + u;

    return r * 7 + i + u * 3L + ch * 5L + l * 7L + sh * 11L;
}

/* A register pointer stepped by ++, --, += and -=, as statements and
   for the values, of each pointee; the pointer is never read through,
   and each value is its offset in bytes from where it started. */
#define PSTEP(E, T, S)                                                  \
    long pstep_##E(register T p S, long n)                              \
    {                                                                   \
        char *start = (char *) p;                                       \
        long r;                                                         \
        p++;                                                            \
        r = (char *) p - start;                                         \
        p--;                                                            \
        --p;                                                            \
        ++p;                                                            \
        ++p;                                                            \
        r = r * 7 + ((char *) p - start);                               \
        p += n;                                                         \
        r = r * 7 + ((char *) p - start);                               \
        p -= n;                                                         \
        p -= 3;                                                         \
        r = r * 7 + ((char *) p - start);                               \
        r = r * 7 + ((char *) p++ - start);                             \
        r = r * 7 + ((char *) ++p - start);                             \
        r = r * 7 + ((char *) p-- - start);                             \
        r = r * 7 + ((char *) --p - start);                             \
        r = r * 7 + ((char *) (p += n) - start);                        \
        r = r * 7 + ((char *) (p -= 2) - start);                        \
        r = r * 7 + ((char *) (p -= n) - start);                        \
        return r * 7 + ((char *) p - start);                            \
    }

POINTEES(PSTEP)

/* *p++, *++p, *p-- and *--p as a load and as a store, on a register
   pointer into the main's buffer; *q++ = *p++ copies; p[i++] and
   p[i]++ with a register i; a local array's a[i]++. */
#define WALK(E, T)                                                      \
    long walk_##E(T *base, long v)                                      \
    {                                                                   \
        register T *p = base;                                           \
        register T *q = base + 20;                                      \
        long a = *p++;                                                  \
        long b = *++p;                                                  \
        long c = *p--;                                                  \
        long d = *--p;                                                  \
        *p++ = v;                                                       \
        *++p = v + 1;                                                   \
        *p-- = v + 2;                                                   \
        *--p = v + 3;                                                   \
        *q++ = *p++;                                                    \
        *q++ = *p++;                                                    \
        *q-- = *p--;                                                    \
        *--q = *--p;                                                    \
        return a + b * 3 + c * 5 + d * 7 + (p - base) * 11              \
               + (q - base) * 13 + *p + *q * 17;                        \
    }                                                                   \
    long index_##E(T *base, int n, long v)                              \
    {                                                                   \
        register int i = 0;                                             \
        register T *p = base;                                           \
        long s = 0;                                                     \
        T a[16];                                                        \
        while (i < n)                                                   \
            s = s * 3 + p[i++];                                         \
        i = 0;                                                          \
        while (i < n) {                                                 \
            p[i]++;                                                     \
            ++p[i];                                                     \
            p[i]--;                                                     \
            i++;                                                        \
        }                                                               \
        for (i = 0; i < 16; i++)                                        \
            a[i] = v + i;                                               \
        for (i = 0; i < n; i++)                                         \
            a[i]++;                                                     \
        for (i = 15; i >= n; --i)                                       \
            a[i] -= 3;                                                  \
        return s * 7 + a[0] + a[15] * 3 + a[n] * 5 + i;                 \
    }

SCALARS(WALK)

/* the classic idioms of the register style */
long rstrlen(char *s)
{
    register char *p = s;

    while (*p++)
        ;

    return p - s - 1;
}

long rstrcpy(char *d, char *s)
{
    register char *p = d;
    register char *q = s;

    while (*p++ = *q++)
        ;

    return p - d;
}

void rmemcpy(char *d, char *s, register long n)
{
    register char *p = d;
    register char *q = s;

    while (n--)
        *p++ = *q++;
}

int rstrcmp(char *a, char *b)
{
    register char *p = a;
    register char *q = b;

    while (*p && *p == *q) {
        p++;
        q++;
    }

    return *(unsigned char *) p - *(unsigned char *) q;
}

/* Loops in the classic form: the counter stepped by ++, the countdown
   while (n--), and a for whose condition is the step, with no do. */
#define LOOPS(E, T)                                                     \
    long loops_##E(T *base, int n)                                      \
    {                                                                   \
        long s = 0;                                                     \
        for (register int i = 0; i < n; i++)                            \
            s += base[i];                                               \
        register int k = n;                                             \
        while (k--)                                                     \
            s = s * 3 + base[k];                                        \
        for (register int j = n; j--;)                                  \
            s = s * 5 + base[j];                                        \
        register T *p = base + n;                                       \
        while (p > base)                                                \
            s = s * 7 + *--p;                                           \
        return s * 7 + k;                                               \
    }                                                                   \
    long sum_##E(register T *p, register int n)                         \
    {                                                                   \
        register long s = 0;                                            \
        while (n--)                                                     \
            s += *p++;                                                  \
        return s;                                                       \
    }

SCALARS(LOOPS)

/* The order of reads and in-place writes, backend.md chapter 4 and
   6.7, G13: left to right, each operand's side effects complete
   before the next starts.  ISO leaves each of these undefined; see the
   main for where clang -O0 agrees. */
#define ORDER(T, N)                                                     \
    long ord_self_##N(T a) { register T i = a; return i + i++; }        \
    long ord_twice_##N(T a) { register T i = a; return i++ + i++; }     \
    long ord_down_##N(T a) { register T i = a; return --i + i--; }      \
    long ord_add_##N(T a) { register T x = a; return x + (x += 2); }    \
    long ord_own_##N(T a)                                               \
    {                                                                   \
        register T r = a;                                               \
        r += (r = 2);                                                   \
        return r;                                                       \
    }                                                                   \
    long ord_cond_##N(T a, int c)                                       \
    {                                                                   \
        register T x = a;                                               \
        return x + (c ? x++ : 2) + x * 3L + (x + 1) * 5L;               \
    }                                                                   \
    long ord_and_##N(T a, int c)                                        \
    {                                                                   \
        register T x = a;                                               \
        return x + (c && (x += 5)) + (c || x--) + x * 3L                \
               + (x + 1) * 5L;                                          \
    }                                                                   \
    long ord_call_##N(T a) { register T x = a; return x + lid(x++); }   \
    long ord_rcc_##N(T a)                                               \
    {                                                                   \
        register T x = a;                                               \
        return x + twice(x--) + x * 3L + (x + 1) * 5L;                  \
    }

TYPES(ORDER)

/* Calls and blocks with in-place writes live across them: a write
   whose right operand is a call, variables stepped then held across a
   call, a block's variables written in place beside the outer's, a
   callee of rcc's that writes its own in place. */
long own_steps(long v)
{
    register long a = v;
    register long b = v * 3;
    register int c = v;
    register char d = v;
    register unsigned e = v;

    a += 5;
    b *= 7;
    c <<= 2;
    d++;
    e -= 9;

    return a + b + c + d + e + (d + 1) * 3L;
}

#define LIVE(T, N)                                                      \
    long live_##N(T a, T b)                                             \
    {                                                                   \
        register T x = a;                                               \
        register T y = b;                                               \
        long r;                                                         \
        x += lid(b);                                                    \
        y++;                                                            \
        r = twice(x) + x + (x + 1) * 3L + (y + 1) * 5L;                 \
        x *= 3;                                                         \
        r = r * 7 + lid(y) + x + y + (x + 1) * 3L;                      \
        r = r * 7 + own_steps(r) + x + y;                               \
        {                                                               \
            register T z = a;                                           \
            z += x;                                                     \
            z <<= 1;                                                    \
            r = r * 7 + twice(z) + z + x + (z + 1) * 3L;                \
        }                                                               \
        {                                                               \
            register T w = b;                                           \
            w -= y;                                                     \
            lid(w--);                                                   \
            r = r * 7 + w + y + (w + 1) * 3L;                           \
            w ^= own_steps(w);                                          \
            r = r * 7 + w + (w + 1) * 3L;                               \
        }                                                               \
        return r * 7 + x + y * 3L;                                      \
    }

TYPES(LIVE)

/* recursion with in-place writes on both sides of the call */
long rcount(register int n, register long acc)
{
    register long m = n;

    m *= 3;
    if (n-- <= 0)
        return acc;

    acc += m;

    return rcount(n, acc) + m - n;
}
