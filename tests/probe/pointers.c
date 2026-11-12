/* The pointers probe: pointer arithmetic and the subscript, in rcc's
   C, for docs/aarch64.md 6.3's rows of p[i] to p - q, the index under
   the table, G6 and G7.  A pointer result is returned as a char *,
   since a pointer to a row has no type name in rcc's C; the cast is
   no instruction.  See pointers.h. */

#include "pointers.h"

/* pointers-lib.c: wide_<n> is a long behind a declaration of the type,
   so its result is stale above the type's width; id() is a call */
#define LIBRARY(T, N) T wide_##N(long x);

TYPES(LIBRARY)

long id(long a);

/* p + n, n + p and p - n, 6.5.7p9, G6: n of each type as a variable,
   and as a call's result, which is valid to its own width only */
#define ARITH(E, T, S, I, N)                                            \
    char *add_##E##_##N(T p S, I n) { return (char *) (p + n); }        \
    char *radd_##E##_##N(T p S, I n) { return (char *) (n + p); }       \
    char *sub_##E##_##N(T p S, I n) { return (char *) (p - n); }        \
    char *cadd_##E##_##N(T p S, long x)                                 \
    {                                                                   \
        return (char *) (p + wide_##N(x));                              \
    }                                                                   \
    char *csub_##E##_##N(T p S, long x)                                 \
    {                                                                   \
        return (char *) (p - wide_##N(x));                              \
    }
#define ARITH_ALL(E, T, S) INDEXES(ARITH, E, T, S)

POINTEES(ARITH_ALL)

/* p + n with n a constant, and p += n */
#define KARITH(E, T, S, name, n)                                        \
    char *kadd_##E##_##name(T p S) { return (char *) (p + (n)); }       \
    char *kradd_##E##_##name(T p S) { return (char *) ((n) + p); }      \
    char *ksub_##E##_##name(T p S) { return (char *) (p - (n)); }       \
    char *kaddeq_##E##_##name(T p S)                                    \
    {                                                                   \
        p += n;                                                         \
        return (char *) p;                                              \
    }                                                                   \
    char *ksubeq_##E##_##name(T p S) { return (char *) (p -= n); }
#define KARITH_ALL(E, T, S) KONSTS(KARITH, E, T, S)

POINTEES(KARITH_ALL)

/* p - q, 6.5.7p10, G6: a division by the size, truncating; ++ and --
   of a pointer and p += n, p -= n, as values and in the effect
   context; G7 on pointers derived by arithmetic */
#define POINTER(E, T, S)                                                \
    long diff_##E(T p S, T q S) { return p - q; }                       \
    int diff_less_##E(T p S, T q S) { return p - q < 0; }               \
    long diff_held_##E(T p S, T q S, long n) { return p + n - q; }      \
    char *preinc_##E(T p S) { return (char *) ++p; }                    \
    char *postinc_##E(T p S) { return (char *) p++; }                   \
    char *predec_##E(T p S) { return (char *) --p; }                    \
    char *postdec_##E(T p S) { return (char *) p--; }                   \
    long post_both_##E(T p S)                                           \
    {                                                                   \
        char *a = (char *) p++;                                         \
        char *b = (char *) p--;                                         \
        return (char *) p - a + ((char *) p - b) * 1000;                \
    }                                                                   \
    char *steps_##E(T p S)                                              \
    {                                                                   \
        ++p;                                                            \
        p++;                                                            \
        --p;                                                            \
        p++;                                                            \
        p++;                                                            \
        return (char *) p;                                              \
    }                                                                   \
    char *addeq_##E(T p S, long n) { return (char *) (p += n); }        \
    char *subeq_##E(T p S, long n) { return (char *) (p -= n); }        \
    char *addeq_int_##E(T p S, int n)                                   \
    {                                                                   \
        p += n;                                                         \
        return (char *) p;                                              \
    }                                                                   \
    char *subeq_unsigned_##E(T p S, unsigned n)                         \
    {                                                                   \
        p -= n;                                                         \
        return (char *) p;                                              \
    }                                                                   \
    char *addeq_char_##E(T p S, signed char n)                          \
    {                                                                   \
        p += n;                                                         \
        p -= n;                                                         \
        p += n;                                                         \
        return (char *) p;                                              \
    }                                                                   \
    int lt_derived_##E(T p S, long n, long m) { return p + n < p + m; } \
    int ge_derived_##E(T p S, long n, T q S) { return p + n >= q; }     \
    int eq_derived_##E(T p S, long n, T q S) { return p + n == q - n; } \
    int le_stepped_##E(T p S, T q S) { return ++p <= q; }               \
    long wrap_##E(T p S, long n) { return (long) (p + n) - (long) p; }

POINTEES(POINTER)

/* The subscript, p[i] and i[p], read, written, under op= and ++ and
   --, for each element size and each index type, the index a variable
   and a call's result; &p[i] and &*(p + i); a local array's a[i]. */
#define SUBSCRIPT(E, T, S, I, N)                                        \
    long get_##E##_##N(T *p, I i) { return p[i]; }                      \
    long rget_##E##_##N(T *p, I i) { return i[p]; }                     \
    void put_##E##_##N(T *p, I i, long v) { p[i] = v; }                 \
    long putv_##E##_##N(T *p, I i, long v) { return p[i] = v; }         \
    long addto_##E##_##N(T *p, I i, T v) { return p[i] += v; }          \
    void addon_##E##_##N(T *p, I i, T v) { p[i] += v; }                 \
    void mulon_##E##_##N(T *p, I i, T v) { p[i] *= v; }                 \
    long inc_##E##_##N(T *p, I i) { return ++p[i]; }                    \
    long postdec_##E##_##N(T *p, I i) { return p[i]--; }                \
    void steps_##E##_##N(T *p, I i)                                     \
    {                                                                   \
        p[i]++;                                                         \
        --p[i];                                                         \
        p[i]++;                                                         \
    }                                                                   \
    char *addr_##E##_##N(T *p, I i) { return (char *) &p[i]; }          \
    char *addr_star_##E##_##N(T *p, I i)                                \
    {                                                                   \
        return (char *) &*(p + i);                                      \
    }                                                                   \
    long cget_##E##_##N(T *p, long x) { return p[wide_##N(x)]; }        \
    void cput_##E##_##N(T *p, long x, long v) { p[wide_##N(x)] = v; }   \
    char *caddr_##E##_##N(T *p, long x)                                 \
    {                                                                   \
        return (char *) &p[wide_##N(x)];                                \
    }                                                                   \
    long aget_##E##_##N(I i, long v)                                    \
    {                                                                   \
        T a[16];                                                        \
        for (int k = 0; k < 16; k++)                                    \
            a[k] = v + k * 3;                                           \
        a[i] += 1;                                                      \
        a[i]++;                                                         \
        return a[i] * 2 + a[0] + a[15];                                 \
    }                                                                   \
    long aaddr_##E##_##N(I i)                                           \
    {                                                                   \
        T a[16];                                                        \
        return (&a[i] - &a[1]) * 100 + ((char *) &a[i] - (char *) a);   \
    }
#define SUBSCRIPT_ALL(E, T, S) INDEXES(SUBSCRIPT, E, T, S)

ELEMENTS(SUBSCRIPT_ALL)

/* a constant subscript on each side of the load's reach, read, written
   and stepped; a constant displacement through a cast to a wider
   element */
#define KSUB(E, T, S, name, c)                                          \
    long kget_##E##_##name(T *p) { return p[c]; }                       \
    void kput_##E##_##name(T *p, long v) { p[c] = v; }                  \
    long kinc_##E##_##name(T *p) { ++p[c]; return p[c]; }
#define KSUB_ALL(E, T, S) KSUBS(KSUB, E, T, S)

ELEMENTS(KSUB_ALL)

#define KBYTE(E, T, S, name, d)                                         \
    long kview_##E##_##name(char *p) { return *(T *) (p + d); }         \
    void kview_put_##E##_##name(char *p, long v) { *(T *) (p + d) = v; } \
    long kview_index_##E##_##name(char *p) { return ((T *) (p + 1))[d]; }
#define KBYTE_ALL(E, T, S) KBYTES(KBYTE, E, T, S)

ELEMENTS(KBYTE_ALL)

/* A pointer in memory stepped, the main's cells holding it; the
   walks, *p++, *++p, *p++ = v, and (*p)++ against *p++; a buffer
   filled and summed by a walking pointer, a comparison of pointers
   the condition. */
#define WALKS(E, T, S)                                                  \
    void cell_steps_##E(T **pp)                                         \
    {                                                                   \
        (*pp)++;                                                        \
        ++*pp;                                                          \
        *pp += 3;                                                       \
        *pp -= 1;                                                       \
        --*pp;                                                          \
    }                                                                   \
    char *cell_post_##E(T **pp) { return (char *) (*pp)++; }            \
    char *cell_pre_##E(T **pp) { return (char *) --*pp; }               \
    char *cell_addeq_##E(T **pp, long n) { return (char *) (*pp += n); } \
    long walk_##E(T *p, long v)                                         \
    {                                                                   \
        long a = *p++;                                                  \
        long b = *++p;                                                  \
        *p++ = v;                                                       \
        long c = (*p)++;                                                \
        long d = *p++;                                                  \
        return a + b * 3 + c * 5 + d * 7 + *p;                          \
    }                                                                   \
    long walk_back_##E(T *p)                                            \
    {                                                                   \
        long a = *p--;                                                  \
        long b = *--p;                                                  \
        *p-- = a;                                                       \
        return a - b + *p;                                              \
    }                                                                   \
    long sum_##E(T *p, T *end)                                          \
    {                                                                   \
        long s = 0;                                                     \
        while (p < end)                                                 \
            s = s * 3 + *p++;                                           \
        return s;                                                       \
    }                                                                   \
    long sum_back_##E(T *p, T *end)                                     \
    {                                                                   \
        long s = 0;                                                     \
        while (end > p)                                                 \
            s = s * 3 + *--end;                                         \
        return s;                                                       \
    }                                                                   \
    long fill_##E(T *p, T *end, long v)                                 \
    {                                                                   \
        while (p != end)                                                \
            *p++ = v++;                                                 \
        return v;                                                       \
    }                                                                   \
    long pp_##E(T **p, long i, long j) { return p[i][j]; }              \
    void pp_put_##E(T **p, int i, int j, long v) { p[i][j] = v; }

SCALARS(WALKS)

/* An array of arrays: m[i][j] on a local and on a parameter, its row
   decayed and the row's address, the difference of two rows; an array
   filled and summed by index and by a walking pointer, forward and
   backward; the frame, an array of more than 4095 bytes and of more
   than 65535 indexed at its ends. */
#define ARRAYS(E, T, S)                                                 \
    long mat_##E(int i, int j, long v)                                  \
    {                                                                   \
        T m[4][5];                                                      \
        for (int r = 0; r < 4; r++)                                     \
            for (int c = 0; c < 5; c++)                                 \
                m[r][c] = v + r * 5 + c;                                \
        m[i][j] += 1;                                                   \
        return m[i][j] * 3 + m[3 - i][4 - j] + *m[i] + (m[3] - m[i]);   \
    }                                                                   \
    long pmat_##E(T m[][5], long i, long j) { return m[i][j]; }         \
    void pmat_put_##E(T m[][5], int i, unsigned j, long v)              \
    {                                                                   \
        m[i][j] = v;                                                    \
    }                                                                   \
    char *prow_##E(T m[][5], long i) { return (char *) m[i]; }          \
    long loop_index_##E(int n, long v)                                  \
    {                                                                   \
        T a[64];                                                        \
        long s = 0;                                                     \
        for (int k = 0; k < n; k++)                                     \
            a[k] = v * k + 1;                                           \
        for (int k = n - 1; k >= 0; k--)                                \
            s = s * 3 + a[k];                                           \
        return s;                                                       \
    }                                                                   \
    long loop_walk_##E(int n, long v)                                   \
    {                                                                   \
        T a[64];                                                        \
        T *end = a + n;                                                 \
        long s = 0;                                                     \
        for (T *q = a; q < end; q++)                                    \
            *q = v + (q - a);                                           \
        for (T *q = end; q > a;)                                        \
            s = s * 3 + *--q;                                           \
        for (T *q = a; q != end; ++q)                                   \
            s += *q;                                                    \
        return s;                                                       \
    }                                                                   \
    long ends_##E(int i, long v)                                        \
    {                                                                   \
        T a[5000];                                                      \
        a[0] = v;                                                       \
        a[4999] = v + 1;                                                \
        a[i] = v + 2;                                                   \
        a[4999 - i] += 3;                                               \
        return a[0] * 3 + a[4999] * 5 + a[i] + (&a[4999] - &a[i]);      \
    }                                                                   \
    long vast_##E(long i, long v)                                       \
    {                                                                   \
        T a[70000];                                                     \
        T x = v;                                                        \
        a[0] = v;                                                       \
        a[69999] = x + 1;                                               \
        a[i] = x - 1;                                                   \
        a[69999 - i]++;                                                 \
        return a[0] * 3 + a[69999] * 5 + a[i] + (&a[69999] - a);        \
    }

SCALARS(ARRAYS)

/* string literals subscripted and walked */
int lit_index(int i) { return "abcdef"[i]; }

int lit_rindex(long i) { return i["\001\177\200\377xyz"]; }

long lit_walk(void)
{
    char *s = "walked to its terminator";
    long h = 0;

    while (*s)
        h = h * 31 + *s++;

    return h;
}

long lit_length(void)
{
    char *s = "seventeen bytes..";
    char *t = s;

    while (*t)
        t++;

    return t - s;
}

/* Depth, 6.2: a subscript with eighteen values held, read and
   written, and an address; an index that is a call, so the base is
   held across it, on a pointer and on a local array. */
long deep_index(P8(long), long *p, int i)
{
    return a - (b - (c - (d - (e - (f - (g - (h - (a - (b - (c - (d - (e -
           (f - (g - (h - (a - (b - p[i])))))))))))))))));
}

long deep_put(P8(long), int *p, long i)
{
    return a - (b - (c - (d - (e - (f - (g - (h - (a - (b - (c - (d - (e -
           (f - (g - (h - (a - (b - (p[i] = a ^ h))))))))))))))))));
}

long deep_addr(P8(long), short *p, int i)
{
    return a * (b + (c * (d + (e * (f + (g * (h + (a * (b + (c * (d + (e *
           (f + (g * (h + (a * (b + (&p[i] - p))))))))))))))))));
}

long deep_diff(P8(long), int *p, int *q)
{
    return a - (b - (c - (d - (e - (f - (g - (h - (a - (b - (c - (d - (e -
           (f - (g - (h - (a - (b - (p - q))))))))))))))))));
}

long call_index(long *p, long i)
{
    return p[id(i)] - p[id(i + 1)] * p[i];
}

long call_put(int *p, long i, long v)
{
    p[id(i)] = id(v);

    return p[i];
}

long call_local(int i, long v)
{
    long a[8];

    for (int k = 0; k < 8; k++)
        a[k] = v - k;

    return a[id(i)] * 2 + a[id(7 - i)];
}

char *call_add(short *p, long n) { return (char *) (p + id(n)); }
