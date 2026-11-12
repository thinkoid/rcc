/* The memory probe: addresses and indirection, in rcc's C, for
   docs/aarch64.md 6.3's rows of &x, *p and "abc", the use of an array,
   and pointers as values.  No pointer arithmetic and no subscript,
   which are the pointers probe's: no + or - with a pointer operand,
   no [] in an expression, no ++, --, += or -= of a pointer.  A pointer
   into the middle of a buffer, or misaligned, is made by the main.
   See memory.h. */

#include "memory.h"

/* memory-lib.c: through a pointer, touch_<n> reads the object and
   writes it, fill_<n> writes n objects and total_<n> sums them; text()
   reads a string; mix1() and mix2() are calls with no memory */
#define LIBRARY(T, N)                                                   \
    long touch_##N(T *p);                                               \
    long fill_##N(T *a, int n, long v);                                 \
    long total_##N(T *a, int n);                                        \
    long fill_rows_##N(T m[][5], int n, long v);                        \
    long total_rows_##N(T m[][5], int n);

TYPES(LIBRARY)

long text(char *s);
long mix1(long a);
long mix2(long a, long b);

/* The load and the store, 6.3, G8: the load at the object's width and
   extension, its value used at 32 bits and at 64; a narrow result,
   which is not extended; the store of a wider value's low bytes, G2,
   for its effect and for its value; memory to memory. */
#define ACCESS(T, N)                                                    \
    long load_##N(T *p) { return *p; }                                  \
    int load_int_##N(T *p) { return *p; }                               \
    long load_add_##N(T *p, long x) { return *p + x; }                  \
    int load_add_int_##N(T *p, int x) { return *p + x; }                \
    long load_shift_##N(T *p) { return *p >> 3; }                       \
    long load_div_##N(T *p) { return *p / 3; }                          \
    int load_less_##N(T *p, T *q) { return *p < *q; }                   \
    T load_result_##N(T *p) { return *p; }                              \
    int load_test_##N(T *p) { if (*p) return 1; return 0; }             \
    int load_not_##N(T *p) { return !*p; }                              \
    void store_##N(T *p, long v) { *p = v; }                            \
    long store_value_##N(T *p, long v) { return *p = v; }               \
    long store_int_##N(T *p, int v) { *p = v; return *p; }              \
    long store_narrow_##N(T *p, signed char v) { *p = v; return *p; }   \
    long copy_##N(T *p, T *q) { *p = *q; return *p; }                   \
    long chain_##N(T *p, T *q, long v) { return *p = *q = v; }

TYPES(ACCESS)

/* a pointer to a pointer: the pointer loaded and dereferenced, and
   stored; the main's cells hold the pointers */
#define TWICE(T, N)                                                     \
    long load2_##N(T **pp) { return **pp; }                             \
    long store2_##N(T **pp, long v) { **pp = v; return **pp; }          \
    T *fetch_##N(T **pp) { return *pp; }                                \
    void retarget_##N(T **pp, T *q) { *pp = q; }                        \
    long through_##N(T **pp, T *q, long v)                              \
    {                                                                   \
        *pp = q;                                                        \
        **pp = v;                                                       \
        return *q;                                                      \
    }

TYPES(TWICE)

/* *p op= e for every operator, for its value and in the effect
   context, 6.6: the load, the operation, the store */
#define COMPOUND(T, N, name, op)                                        \
    long name##_eq_##N(T *p, T e) { return *p op e; }                   \
    void name##_on_##N(T *p, T e) { *p op e; }
#define COMPOUND_ALL(T, N) COMPOUNDS(COMPOUND, T, N)

TYPES(COMPOUND_ALL)

#define SHIFT(T, N, name, op)                                           \
    long name##_eq_##N(T *p, int n) { return *p op n; }                 \
    void name##_on_##N(T *p, int n) { *p op n; }
#define SHIFT_ALL(T, N) SHIFTS(SHIFT, T, N)

TYPES(SHIFT_ALL)

/* a right operand wider than the object: the operation in long */
#define WIDENED(T, N)                                                   \
    long wide_add_##N(T *p, long e) { return *p += e; }                 \
    void ewide_xor_##N(T *p, long e) { *p ^= e; }

TYPES(WIDENED)

#define KCOMPOUND(T, N, name, op, constant)                             \
    long k##name##_##N(T *p) { return *p op constant; }                 \
    void ek##name##_##N(T *p) { *p op constant; }
#define KCOMPOUND_ALL(T, N) KCOMPOUNDS(KCOMPOUND, T, N)

TYPES(KCOMPOUND_ALL)

/* ++ and -- through a pointer, as values and in the effect context */
#define STEPS(T, N)                                                     \
    long preinc_##N(T *p) { return ++*p; }                              \
    long postinc_##N(T *p) { return (*p)++; }                           \
    long predec_##N(T *p) { return --*p; }                              \
    long postdec_##N(T *p) { return (*p)--; }                           \
    void steps_##N(T *p, T *q)                                          \
    {                                                                   \
        ++*p;                                                           \
        (*p)++;                                                         \
        --*q;                                                           \
        (*q)--;                                                         \
        (*q)--;                                                         \
    }                                                                   \
    long stepped_##N(T *p)                                              \
    {                                                                   \
        long a = ++*p;                                                  \
        long b = (*p)++;                                                \
        long c = (*p)--;                                                \
        return a + b * 3 + c * 5 + *p;                                  \
    }

TYPES(STEPS)

/* a constant stored through a pointer in the effect context, the
   store row of 6.6, and for its value */
#define STORE(T, N, name, constant)                                     \
    void k##name##_store_##N(T *p) { *p = constant; }                   \
    long k##name##_value_##N(T *p) { return *p = constant; }
#define STORE_ALL(T, N) STORES(STORE, T, N)

TYPES(STORE_ALL)

/* &x of a local and of a parameter, passed to the library, which
   reads and writes through it, and the object read after; &x in a
   pointer local; *&x and &*p; a pointer to a pointer to a local */
#define ADDRESS(T, N)                                                   \
    long addr_local_##N(T a)                                            \
    {                                                                   \
        T x = a;                                                        \
        long r = touch_##N(&x);                                         \
        return r + x;                                                   \
    }                                                                   \
    long addr_param_##N(T a)                                            \
    {                                                                   \
        long r = touch_##N(&a);                                         \
        return r + a;                                                   \
    }                                                                   \
    long addr_held_##N(T a)                                             \
    {                                                                   \
        T x = a;                                                        \
        T *q = &x;                                                      \
        *q += 1;                                                        \
        return x + *q;                                                  \
    }                                                                   \
    long star_amp_##N(T a)                                              \
    {                                                                   \
        T x = a;                                                        \
        *&x = a + 1;                                                    \
        return *&x + *&a;                                               \
    }                                                                   \
    T *amp_star_##N(T *p) { return &*p; }                               \
    long amp_star_store_##N(T *p, long v)                               \
    {                                                                   \
        *&*p = v;                                                       \
        return *&*p;                                                    \
    }                                                                   \
    long addr_pp_##N(T a)                                               \
    {                                                                   \
        T x = a;                                                        \
        T *q = &x;                                                      \
        T **qq = &q;                                                    \
        **qq += 2;                                                      \
        long r = touch_##N(*qq);                                        \
        return r + x + **qq;                                            \
    }                                                                   \
    long addr_two_##N(T a, T b)                                         \
    {                                                                   \
        T x = a;                                                        \
        T y = b;                                                        \
        T *p = &x;                                                      \
        T *q = &y;                                                      \
        *p = *q;                                                        \
        *q = a;                                                         \
        return x * 3 + y;                                               \
    }

TYPES(ADDRESS)

/* an array: decayed as an argument, to be filled and summed, assigned
   to a pointer local, its first element read and written through *a;
   an array of arrays decayed, its first row decayed, and **m */
#define ARRAYS(T, N)                                                    \
    long array_##N(T v)                                                 \
    {                                                                   \
        T a[4];                                                         \
        T *p = a;                                                       \
        *p = v;                                                         \
        *a += 1;                                                        \
        return *a + *p;                                                 \
    }                                                                   \
    long array_fill_##N(int n, long v)                                  \
    {                                                                   \
        T a[16];                                                        \
        long r = fill_##N(a, n, v);                                     \
        return r + total_##N(a, n) + *a;                                \
    }                                                                   \
    long matrix_##N(long v)                                             \
    {                                                                   \
        T m[3][5];                                                      \
        long r = fill_rows_##N(m, 3, v);                                \
        **m += 1;                                                       \
        r += fill_##N(*m, 2, v * 3);                                    \
        return r + **m + total_rows_##N(m, 3);                          \
    }

TYPES(ARRAYS)

/* String literals, chapter 10: the address passed to the library,
   which reads it; stored in a char * and its first byte read; *"abc";
   the same text in two places, whose two labels nothing compares;
   every kind of byte .string writes */
long text_pass(void) { return text("hello, world"); }

long text_two(void) { return text("same") * 3 + text("same"); }

long text_escapes(void)
{
    return text("\t\n\r\\\"\001\037\177\200\377 'end'");
}

long text_empty(void) { return text("") + *""; }

int text_first(void)
{
    char *s = "abc";

    return *s;
}

int text_star(void) { return *"xyz" * 3 + *"xyz"; }

long text_held(long a)
{
    char *s = "q";
    char *t = "r";

    return *s * a + *t;
}

int text_cond(int c)
{
    char *s = c ? "yes" : "no";

    return *s;
}

long text_arg(int c) { return text(c ? "one" : "two") + text("three"); }

long text_joined(void) { return text("two " "literals" " in one"); }

/* Pointers as values, G7: parameters past the eighth, results, the
   comparisons, the unsigned conditions, against a null pointer
   constant, as a condition, under !, in && and || and ?:. */
#define VALUES(T, N)                                                    \
    long ninth_##N(P8(long), T *p, T *q)                                \
    {                                                                   \
        *q = *p + a;                                                    \
        return *p - h;                                                  \
    }                                                                   \
    T *pick_##N(T *p, T *q, int c) { return c ? p : q; }                \
    T *pick9_##N(P8(long), T *p) { return p; }                          \
    int eq_##N(T *p, T *q) { return p == q; }                           \
    int ne_##N(T *p, T *q) { return p != q; }                           \
    int lt_##N(T *p, T *q) { return p < q; }                            \
    int le_##N(T *p, T *q) { return p <= q; }                           \
    int gt_##N(T *p, T *q) { return p > q; }                            \
    int ge_##N(T *p, T *q) { return p >= q; }                           \
    int iflt_##N(T *p, T *q) { if (p < q) return 1; return 2; }         \
    int ifge_##N(T *p, T *q) { if (p >= q) return 1; return 2; }        \
    int ifeq_##N(T *p, T *q) { if (p == q) return 1; return 2; }        \
    int null_##N(T *p) { return p == 0; }                               \
    int nonnull_##N(T *p) { return 0 != p; }                            \
    int truth_##N(T *p) { if (p) return 1; return 0; }                  \
    int not_##N(T *p) { return !p; }                                    \
    int and_##N(T *p, T *q) { return p && q; }                          \
    int or_##N(T *p, T *q) { return p || q; }                           \
    int both_##N(T *p, T *q) { if (p && !q) return 1; return 0; }       \
    long sel_##N(T *p, long a, long b) { return p ? a : b; }            \
    T *either_##N(T *p, T *q) { return p ? p : q; }                     \
    long deref_pick_##N(T *p, T *q)                                     \
    {                                                                   \
        *pick_##N(p, q, 0) = 5;                                         \
        return *pick_##N(p, q, 1) * 7 + *pick_##N(p, q, 0);             \
    }

TYPES(VALUES)

/* conversions: a pointer to long, unsigned long and int and back, an
   integer to a pointer by its own signedness, pointer to pointer
   through void *; and G10, an object read and written through a
   pointer of another type */
#define CONVERSIONS(T, N)                                               \
    long to_long_##N(T *p) { return (long) p; }                         \
    unsigned long to_ulong_##N(T *p) { return (unsigned long) p; }      \
    int to_int_##N(T *p) { return (int) p; }                            \
    T *from_long_##N(long a) { return (T *) a; }                        \
    T *from_ulong_##N(unsigned long a) { return (T *) a; }              \
    T *from_int_##N(int a) { return (T *) a; }                          \
    T *from_unsigned_##N(unsigned a) { return (T *) a; }                \
    T *from_char_##N(signed char a) { return (T *) a; }                 \
    T *round_##N(T *p) { return (T *) (unsigned long) (long) p; }       \
    T *voided_##N(T *p)                                                 \
    {                                                                   \
        void *v = p;                                                    \
        T *q = v;                                                       \
        return q;                                                       \
    }                                                                   \
    long view_load_##N(char *p) { return *(T *) p; }                    \
    void view_store_##N(int *p, long v) { *(T *) p = v; }               \
    long view_back_##N(T *p)                                            \
    {                                                                   \
        char *c = (char *) p;                                           \
        unsigned char *u = (unsigned char *) c;                         \
        long *l = (long *) u;                                           \
        return *c + *u * 3 + (*l & 0xffff) + *(int *) l;                \
    }

TYPES(CONVERSIONS)

/* The frame, 5.3: &x and an access through a pointer held in a local,
   in frames of more than 4095 bytes and of more than 65535; the
   arrays are declared first and never used, so the slots after them
   are beyond reach */
#define FRAMES(T, N)                                                    \
    long far_##N(T a)                                                   \
    {                                                                   \
        char pad[5000];                                                 \
        T x = a;                                                        \
        T *q = &x;                                                      \
        long r = touch_##N(q);                                          \
        *q += 1;                                                        \
        return r + x + *q;                                              \
    }                                                                   \
    long farther_##N(T a)                                               \
    {                                                                   \
        char pad[70000];                                                \
        T x = a;                                                        \
        T *q = &x;                                                      \
        long r = touch_##N(&x);                                         \
        *q += 1;                                                        \
        return r + x + *q;                                              \
    }                                                                   \
    long far_held_##N(T *p, T a)                                        \
    {                                                                   \
        char pad[5000];                                                 \
        T *q = p;                                                       \
        *q += a;                                                        \
        return *q;                                                      \
    }                                                                   \
    long farther_held_##N(T *p, T a)                                    \
    {                                                                   \
        char pad[70000];                                                \
        T *q = p;                                                       \
        T **qq = &q;                                                    \
        **qq *= a;                                                      \
        return *q + **qq;                                               \
    }

TYPES(FRAMES)

/* an array itself beyond reach, decayed */
long far_array(int n, long v)
{
    char pad[70000];
    long a[16];
    long r = fill_l(a, n, v);

    return r + total_l(a, n) + *a;
}

long far_matrix(long v)
{
    int pad[1200];
    int m[3][5];
    long r = fill_rows_i(m, 3, v);

    **m -= 7;

    return r + **m + total_rows_i(m, 3);
}

/* a parameter's address in a frame beyond reach */
long far_param(long a, int b)
{
    char pad[70000];
    long r = touch_l(&a) + touch_i(&b);

    return r + a + b;
}

/* Depth, 6.2: a dereference with eighteen values held, a load and a
   store; and a dereference on each side of a call, the value of the
   first held across it */
long deep_load(P8(long), long *p)
{
    return a - (b - (c - (d - (e - (f - (g - (h - (a - (b - (c - (d - (e -
           (f - (g - (h - (a - (b - *p)))))))))))))))));
}

long deep_store(P8(long), int *p)
{
    return a - (b - (c - (d - (e - (f - (g - (h - (a - (b - (c - (d - (e -
           (f - (g - (h - (a - (b - (*p = a ^ h))))))))))))))))));
}

long deep_twice(P8(long), short **pp)
{
    return a * (b + (c * (d + (e * (f + (g * (h + (a * (b + (c * (d + (e *
           (f + (g * (h + (a * (b + **pp)))))))))))))))));
}

long around(long *p, long *q, long a)
{
    return *p + mix1(a) * *q - *p;
}

long around_held(int *p, long a)
{
    long x = *p;

    return x + mix1(*p) + *p * mix2(a, *p);
}

long around_store(char *p, long a)
{
    *p = mix1(a) + *p;

    return *p - mix1(*p);
}
