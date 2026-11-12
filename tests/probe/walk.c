/* The walk probe: an access through a register pointer that steps it,
   in rcc's C, for docs/aarch64.md 2.3 and 6.3: *p++, *p--, *++p, *--p,
   *(p += c) and *(p -= c) on a register pointer to each pointee, as a
   load, as a store and as both; both ends of *p++ = *q++ and the
   string idioms; a register pointer to pointers; stores whose right
   operand names the pointer; compound assignment and ++ and -- through
   the stepped pointer; constants in the form's reach and past it; a
   read of the pointer held across the access and across a branch; the
   value discarded; and accesses the form does not take.  Each function
   returns the pointer's final position, p - base, beside the values it
   read, so a step of the wrong size or at the wrong time shows.  A
   function keeps to five register variables live at once, x86-64's
   count.  See walk.h.

   A pointer a function computes is returned as its offset from base,
   never whole: each build has a buffer of its own, at another address.
   A pointer loaded from the buffer is returned whole, since its bytes
   are the main's, alike in both. */

#include "walk.h"

/* walk-lib.c */
long lid(long a);

/* rcc's own callee, in rcc's build */
long twice(long v)
{
    return v * 2 + 1;
}

/* *p++, *p--, *++p and *--p as a load, each value read whole and the
   position after it */
#define LOAD(T, N)                                                      \
    long load_##N(T *base)                                              \
    {                                                                   \
        register T *p = base;                                           \
        long r = (long) *p++;                                           \
        r = r * 31 + (p - base);                                        \
        r = r * 31 + (long) *p++;                                       \
        r = r * 31 + (p - base);                                        \
        r = r * 31 + (long) *p--;                                       \
        r = r * 31 + (p - base);                                        \
        r = r * 31 + (long) *++p;                                       \
        r = r * 31 + (p - base);                                        \
        r = r * 31 + (long) *--p;                                       \
        r = r * 31 + (long) *--p;                                       \
        r = r * 31 + (p - base);                                        \
        r = r * 31 + (long) *++p;                                       \
        return r * 31 + (p - base);                                     \
    }

/* The four as a store: of a constant, of zero, of a slot, of a
   register variable, of a call's result, clang's and rcc's; as
   statements, and for the value stored. */
#define STORE(T, N)                                                     \
    long store_##N(T *base, long v, long w)                             \
    {                                                                   \
        register T *p = base;                                           \
        register T x = (T) w;                                           \
        T s = (T) v;                                                    \
        long r;                                                         \
        *p++ = (T) 85;                                                  \
        *p++ = 0;                                                       \
        *p++ = s;                                                       \
        *p++ = x;                                                       \
        *p++ = (T) lid(v);                                              \
        *p++ = (T) twice(w);                                            \
        r = p - base;                                                   \
        *p-- = (T) 86;                                                  \
        *p-- = 0;                                                       \
        *p-- = s;                                                       \
        *p-- = x;                                                       \
        *p-- = (T) lid(w);                                              \
        r = r * 31 + (p - base);                                        \
        *++p = (T) 87;                                                  \
        *++p = 0;                                                       \
        *++p = s;                                                       \
        *++p = x;                                                       \
        *++p = (T) twice(v);                                            \
        r = r * 31 + (p - base);                                        \
        *--p = (T) 88;                                                  \
        *--p = 0;                                                       \
        *--p = s;                                                       \
        *--p = x;                                                       \
        *--p = (T) lid(v + 1);                                          \
        r = r * 31 + (p - base);                                        \
        r = r * 31 + (long) (*p++ = x);                                 \
        r = r * 31 + (long) (*p++ = 0);                                 \
        r = r * 31 + (long) (*--p = s);                                 \
        r = r * 31 + (long) (*++p = (T) lid(w));                        \
        r = r * 31 + (long) (*p-- = (T) 89);                            \
        return r * 31 + (p - base);                                     \
    }

/* both ends stepped, each pointer by each form */
#define ENDS(T, N)                                                      \
    long ends_##N(T *base)                                              \
    {                                                                   \
        register T *p = base;                                           \
        register T *q = base + 20;                                      \
        long r;                                                         \
        *p++ = *q++;                                                    \
        *p++ = *q++;                                                    \
        *--p = *--q;                                                    \
        *p++ = *--q;                                                    \
        *p-- = *q++;                                                    \
        *++p = *q--;                                                    \
        r = (long) (*p++ = *q++);                                       \
        r = r * 31 + (long) (*--p = *--q);                              \
        r = r * 31 + (long) (*p++ = *--q);                              \
        return r * 31 + (p - base) * 100 + (q - base);                  \
    }

/* The stores whose right operand names the pointer, which keep the
   step before the right operand (G13): the right operand reads the
   pointer as stepped.  The last of each kind is a compound assignment,
   whose two accesses come after the right operand.  ISO leaves each
   undefined, and clang -O0 evaluates such a right operand first; the
   main computes G13's order itself. */
#define NAMES(T, N)                                                     \
    long names_##N(T *base)                                             \
    {                                                                   \
        register T *p = base;                                           \
        long r;                                                         \
        *p++ = *p;                                                      \
        *p++ = (T) (p - base);                                          \
        *++p = *p;                                                      \
        *p++ = *p++;                                                    \
        *p-- = (T) lid((long) p - (long) base);                         \
        *--p = (T) ((long) p - (long) base);                            \
        *p++ += (long) *p;                                              \
        r = p - base;                                                   \
        r = r * 31 + (long) (*p++ = *p);                                \
        r = r * 31 + (long) (*++p = (T) (p - base));                    \
        r = r * 31 + (long) (*p-- = *p--);                              \
        r = r * 31 + (long) (*p-- -= (long) *p);                        \
        return r * 31 + (p - base);                                     \
    }

/* Two accesses through one stepped address: compound assignment and
   ++ and --, for their effect and for their value.  names_<n> has the
   compound assignment whose right operand names the pointer. */
#define RMW(T, N)                                                       \
    long rmw_##N(T *base, long v)                                       \
    {                                                                   \
        register T *p = base;                                           \
        T e = (T) v;                                                    \
        long r;                                                         \
        *p++ += e;                                                      \
        *--p -= e;                                                      \
        *++p <<= 1;                                                     \
        (*p++)++;                                                       \
        --*--p;                                                         \
        ++*p++;                                                         \
        *p++ += 5;                                                      \
        *p-- *= 3;                                                      \
        r = p - base;                                                   \
        r = r * 31 + (long) (*p++ += e);                                \
        r = r * 31 + (long) (*--p -= e);                                \
        r = r * 31 + (long) (*++p <<= 1);                               \
        r = r * 31 + (long) (*p++)++;                                   \
        r = r * 31 + (long) (*p--)--;                                   \
        r = r * 31 + (long) --*--p;                                     \
        r = r * 31 + (long) ++*p++;                                     \
        r = r * 31 + (long) (*p-- *= 3);                                \
        return r * 31 + (p - base);                                     \
    }

INTEGERS(RMW)

/* the same on a pointee that is a pointer, with the operators a
   pointer takes */
long rmw_p(char **base, long v)
{
    register char **p = base;
    long r;

    *p++ += v;
    *--p -= 3;
    (*p++)++;
    --*--p;
    ++*p++;
    *p-- += 5;
    r = p - base;
    r = r * 31 + (long) (*p++ += v);
    r = r * 31 + (long) (*--p -= 3);
    r = r * 31 + (long) (*p++)++;
    r = r * 31 + (long) (*p--)--;
    r = r * 31 + (long) --*--p;
    r = r * 31 + (long) ++*p++;

    return r * 31 + (p - base);
}

/* *(p += c) and *(p -= c) as a load, as a store and as both */
#define JUMP(T, N, c)                                                   \
    long jump_##N##_##c(T *base, long v)                                \
    {                                                                   \
        register T *p = base;                                           \
        long r = (long) *(p += c);                                      \
        r = r * 31 + (long) *(p -= c);                                  \
        r = r * 31 + (long) *(p -= c);                                  \
        r = r * 31 + (p - base);                                        \
        *(p += c) = (T) v;                                              \
        *(p += c) = 0;                                                  \
        *(p -= c) = (T) lid(v);                                         \
        r = r * 31 + (long) (*(p -= c) = (T) v);                        \
        *(p += c) += 3;                                                 \
        r = r * 31 + (long) (*(p -= c) -= 2);                           \
        return r * 31 + (p - base);                                     \
    }
#define JUMPS(T, N) NEAR(JUMP, T, N)

POINTEES(JUMPS)
FAR(JUMP)

/* A read of the pointer held across the access, across a call, and
   across the paths of a branch whose arm steps it.  ISO leaves each
   undefined; G13 orders it left to right. */
#define HELD(T, N)                                                      \
    long held_##N(T *base, int c)                                       \
    {                                                                   \
        register T *p = base;                                           \
        long r = (long) p + (long) *p++ - (long) base;                  \
        r = r * 31 + (p - base) * 100 + (long) *++p;                    \
        r = r * 31 + (p - base) + (c ? (long) *p++ : 0);                \
        r = r * 31 + (p - base) + (c && *p++);                          \
        r = r * 31 + ((long) p - (long) base) + lid((long) *p--);       \
        r = r * 31 + (p - base) + (c || *--p);                          \
        r = r * 31 + ((long) p - (long) base) * 3 + (long) (*p++ = 0);  \
        r = r * 31 + ((long) p + (c ? (long) *p++ : 0) - (long) base);  \
        r = r * 31 + ((long) p + (c && *--p) - (long) base);            \
        r = r * 31 + ((long) p + lid((long) *(p += 2)) - (long) base);  \
        return r * 31 + (p - base);                                     \
    }

/* the value discarded: a statement, a for's step, a comma's left */
#define DISCARD(T, N)                                                   \
    long discard_##N(T *base, int n)                                    \
    {                                                                   \
        register T *p = base;                                           \
        int k;                                                          \
        long r;                                                         \
        *p++;                                                           \
        *--p;                                                           \
        *++p;                                                           \
        *p--;                                                           \
        *(p += 2);                                                      \
        for (k = n; k > 0; *p++)                                        \
            k--;                                                        \
        r = (*p++, p - base);                                           \
        r = r * 31 + (*--p, *(p -= 2), p - base);                       \
        return r * 31 + (p - base);                                     \
    }

/* Not the form: a pointer in a slot stepped; the subscript of a
   stepped register pointer, whose step is computed first; &*p++, which
   makes no access. */
#define OTHER(T, N)                                                     \
    long other_##N(T *base, long v)                                     \
    {                                                                   \
        T *q = base;                                                    \
        register T *p = base + 4;                                       \
        T *a;                                                           \
        long r = (long) *q++;                                           \
        *q++ = (T) v;                                                   \
        r = r * 31 + (long) *--q;                                       \
        *++q = 0;                                                       \
        r = r * 31 + (long) (*q-- += 3);                                \
        r = r * 31 + (q - base);                                        \
        r = r * 31 + (long) p++[2];                                     \
        p++[1] = (T) v;                                                 \
        r = r * 31 + (long) (--p)[-1];                                  \
        a = &*p++;                                                      \
        r = r * 31 + (a - base) + (p - base) * 100;                     \
        a = &*--p;                                                      \
        return r * 31 + (a - base) + (p - base) * 100;                  \
    }

#define POINTEE(T, N)                                                   \
    LOAD(T, N) STORE(T, N) ENDS(T, N) NAMES(T, N) HELD(T, N)            \
    DISCARD(T, N) OTHER(T, N)

POINTEES(POINTEE)

/* the string idioms of the register style: a copy, a length, a compare
   and a reverse */
long wcopy(char *d, char *s)
{
    register char *p = d;
    register char *q = s;

    while (*p++ = *q++)
        ;

    return p - d;
}

long wlength(char *s)
{
    register char *p = s;

    while (*p++)
        ;

    return p - s - 1;
}

int wcompare(char *a, char *b)
{
    register char *p = a;
    register char *q = b;
    register int c;

    while ((c = *p++) == *q++)
        if (c == 0)
            return 0;

    return c - *--q;
}

void wreverse(char *s, long n)
{
    register char *p = s;
    register char *q = s + n;
    register char c;

    while (p < q) {
        c = *p;
        *p++ = *--q;
        *q = c;
    }
}

/* A register pointer to pointers: each form on the outer pointer, and
   on the inner one through it, as a load and as a store; the main's
   cells are the pointers. */
long cells(register char **pp)
{
    char **start = pp;
    long r = **pp++;

    r = r * 31 + *(*pp++)++;
    r = r * 31 + *++*pp++;
    r = r * 31 + **--pp;
    r = r * 31 + *(*--pp)--;
    **pp++ = 7;
    *(*pp++)++ = 9;
    *--*pp++ = 11;
    **++pp = 13;

    return r * 31 + (pp - start);
}

/* A register pointer to a row of 12 bytes, stepped and subscripted: the
   row is no scalar, so no access through the stepped pointer itself. */
long row(register int m[][3], long v)
{
    char *start = (char *) m;
    long r = (*m++)[1];

    r = r * 31 + m++[0][2];
    r = r * 31 + (*--m)[0];
    (*m++)[2] = (int) v;
    m++[1][0] = (int) v + 1;
    r = r * 31 + (*++m)[1];
    r = r * 31 + **m++;
    **--m = 5;

    return r * 31 + ((char *) m - start);
}
