/* The memory probe: addresses and indirection, in rcc's C, for
   docs/aarch64.md 6.3's rows of &x, *p and "abc", the use of an array,
   and pointers as values.  No pointer arithmetic and no subscript,
   which are the pointers probe's: no + or - with a pointer operand,
   no [] in an expression, no ++, --, += or -= of a pointer; but for
   the address of a local array or a string literal plus a constant,
   folded into the slot's or the label's operand, a local array's
   element by a variable index, the slot the indexed operand's
   displacement, a constant over a row's address, a local's or one
   through a pointer to rows, the constant the displacement of that
   address, and an object read under a cast that widens it, an element
   by a subscript among them (backend.md 6.3), and an object in memory
   as an operator's right operand or a condition's object, an element
   by a subscript among them (6.1, 7.1), and the pinned instructions'
   operands, a subscript by a division and an element as an
   assignment's place among them (6.5), which the last sections probe.
   A pointer into the middle of a buffer, or misaligned, is made by the
   main.  See memory.h. */

#include "memory.h"

/* memory-lib.c: through a pointer, touch_<n> reads the object and
   writes it, fill_<n> writes n objects and total_<n> sums them; text()
   reads a string; mix1() and mix2() are calls with no memory */
#define LIBRARY(T, N)                                                   \
    long touch_##N(T *p);                                               \
    long fill_##N(T *a, int n, long v);                                 \
    long total_##N(T *a, int n);                                        \
    long fill_rows_##N(T m[][5], int n, long v);                        \
    long total_rows_##N(T m[][5], int n);                               \
    T *same_##N(T *p);

TYPES(LIBRARY)

/* an object of each type the file only declares, which the library
   defines */
#define DECLARED(T, N) extern T declared_##N;

TYPES(DECLARED)

long text(char *s);
long mix1(long a);
long mix2(long a, long b);
int turn(int i);

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

/* An element of a local array by a constant, backend.md 6.3: the
   slot's operand with the offset folded in, and no instruction forming
   the array's address first.  Read and written, a constant and zero
   stored, operated on in place for the effect and for the value; an
   element of an array of arrays, by a negative column and by one past
   a row's end, inside the array; through the sum of the array and a
   constant, of a constant and the array, of an element's address and a
   constant, and their difference from a constant; the addresses as
   their distance in bytes from the array's start; and in frames that
   put the array beyond the reach of an aarch64 load, 5.3: a char's
   past 4095 bytes, and every type's past 65535. */
#define FOLDS(T, N)                                                     \
    long fold_##N(T v)                                                  \
    {                                                                   \
        T a[8];                                                         \
        long r = fill_##N(a, 8, v);                                     \
                                                                        \
        a[3] = v;                                                       \
        a[2] = 0;                                                       \
        a[4] = 0x12345;                                                 \
        a[5] += v;                                                      \
        a[7] ^= 0x55;                                                   \
        a[1] <<= 3;                                                     \
        a[6]++;                                                         \
        --a[0];                                                         \
        return r + a[0] * 7 + a[1] + a[3] * 3 + a[5] - a[6] + a[7] +    \
               total_##N(a, 8);                                         \
    }                                                                   \
    long fold_value_##N(T v)                                            \
    {                                                                   \
        T a[4];                                                         \
        long r = fill_##N(a, 4, v);                                     \
        long x = a[2]++;                                                \
        long y = a[1] += v;                                             \
        long z = --a[3];                                                \
        long w = a[0] = v * 3;                                          \
                                                                        \
        return r + x * 3 + y * 5 + z * 7 + w * 11 + total_##N(a, 4);    \
    }                                                                   \
    long fold_matrix_##N(long v)                                        \
    {                                                                   \
        T m[3][5];                                                      \
        long r = fill_rows_##N(m, 3, v);                                \
                                                                        \
        m[1][-1] += 2;                                                  \
        m[1][5] = v;                                                    \
        m[2][2]++;                                                      \
        return r + m[1][2] * 3 + m[0][4] + m[2][0] * 5 +                \
               (&m[1][0])[3] + *(m[2] + 4) * 7 + (*(m + 1))[1] +        \
               total_rows_##N(m, 3);                                    \
    }                                                                   \
    long fold_sum_##N(T v)                                              \
    {                                                                   \
        T a[8];                                                         \
        long r = fill_##N(a, 8, v);                                     \
                                                                        \
        *(a + 5) = v;                                                   \
        (a + 2)[3] += 1;                                                \
        *(&a[1] + 1) -= 2;                                              \
        return r + *(3 + a) + (&a[6])[-4] * 3 + *(a + 7 - 1) +          \
               *(a - -1) * 5 + total_##N(a, 8);                         \
    }                                                                   \
    long fold_zero_##N(T v)                                             \
    {                                                                   \
        T a[4];                                                         \
        T m[3][5];                                                      \
        T x = v;                                                        \
        long r = fill_##N(a, 4, v) + fill_rows_##N(m, 3, v);            \
                                                                        \
        *a += 3;                                                        \
        *m[1] = v;                                                      \
        **m -= 1;                                                       \
        *&x += 2;                                                       \
        return r + *a * 3 + *m[1] * 5 + **m * 7 + *&x + *m[2] +         \
               total_##N(a, 4) + total_rows_##N(m, 3);                  \
    }                                                                   \
    long fold_distance_##N(void)                                        \
    {                                                                   \
        T a[8];                                                         \
        T m[3][5];                                                      \
                                                                        \
        return (long) &a[5] - (long) a +                                \
               ((long) (a + 7) - (long) a) * 100 +                      \
               ((long) &m[2][-1] - (long) m) * 10000 +                  \
               ((long) m[2] - (long) m) * 1000000 +                     \
               ((long) (a + 8) - (long) &a[-1]) * 100000000 +           \
               (&a[5] - &a[1]) * 10000000000;                           \
    }                                                                   \
    long far_fold_##N(T v)                                              \
    {                                                                   \
        char pad[4090];                                                 \
        T a[16];                                                        \
        long r = fill_##N(a, 16, v);                                    \
                                                                        \
        a[3] = v;                                                       \
        a[10] += v;                                                     \
        a[15]++;                                                        \
        a[12] = 0;                                                      \
        return r + a[3] + a[10] * 3 + a[15] * 5 +                       \
               ((long) &a[12] - (long) a) * 7 + total_##N(a, 16);       \
    }                                                                   \
    long farther_fold_##N(T v)                                          \
    {                                                                   \
        char pad[70000];                                                \
        T a[8];                                                         \
        T m[3][5];                                                      \
        long r = fill_##N(a, 8, v) + fill_rows_##N(m, 3, v);            \
                                                                        \
        a[3] = v;                                                       \
        a[5] += v;                                                      \
        a[6]++;                                                         \
        m[1][-1] -= 3;                                                  \
        m[2][1] = 0;                                                    \
        return r + a[3] + a[5] * 3 + a[6] * 5 + m[1][2] * 7 + m[0][4] + \
               ((long) (a + 7) - (long) a) + total_##N(a, 8) +          \
               total_rows_##N(m, 3);                                    \
    }

TYPES(FOLDS)

/* an array of pointers: an element by a constant, written, read,
   stepped in place and dereferenced, and subscripted by a constant in
   its turn; q8 points 8 bytes past p */
long fold_pointers(long *p, long *q8, long v)
{
    long *q[4];

    q[1] = p;
    q[2] = q8;
    q[3] = q[1];
    q[0] = 0;
    *q[2] = v;
    q[3]++;
    *q[3] += 5;

    return *q[1] + q[2][0] * 3 + q[1][1] * 5 + (q[0] == 0) * 7;
}

/* a string literal's element by a constant, the label's operand with
   the offset folded in, its terminating zero among them, and *"abc",
   the constant 0; its address, passed to the library, one past the end
   included; and an address beyond the literal, which is no fold, and
   back into it */
int text_elem(void)
{
    return "abcdef"[2] * 3 + "xyz"[3] + *("xyz" + 1) + *"abc" * 5;
}

long text_addr(void)
{
    return text(&"hello, world"[7]) + text("hello" + 2) * 3 +
           text(1 + "abc") + text("hello" + 5) * 5;
}

long text_beyond(void)
{
    char *p = "abcdef" + 9;

    return text(p - 8) + text("abcdef" + 9 - 4) * 3;
}

/* An element of a local array by a variable index, backend.md 6.3, a
   slot and an index: the slot the displacement of the indexed operand
   and rbp its base on x86-64, no instruction forming the array's
   address.  Read and written, a constant and zero stored, operated on
   in place for the effect and for the value, ++ and -- either side; by
   i[a]; by an index that is an expression with a call in it, and with
   a call in the right operand, each held across the call (6.7); by a
   register variable; an element of a row by a constant, m[1][i]; and
   the rows that keep the array's address formed: a pointer, p[i], a row
   of an array of arrays by a variable, whose size is no scale, and
   a - i.  The addresses as their distance in bytes from the array's
   start.  i is 0 to 7. */
#define INDEXED(T, N)                                                   \
    long indexed_##N(T v, int i)                                        \
    {                                                                   \
        T a[8];                                                         \
        T *p = a;                                                       \
        long r = fill_##N(a, 8, v);                                     \
                                                                        \
        a[i] = v;                                                       \
        a[i ^ 2] = 0;                                                   \
        a[i ^ 4] = 0x12345;                                             \
        a[7 - i] += v;                                                  \
        a[i ^ 5] ^= 0x55;                                               \
        a[i ^ 3] <<= 3;                                                 \
        a[i ^ 6]++;                                                     \
        --a[i ^ 1];                                                     \
        p[i ^ 7] -= 9;                                                  \
        return r + a[i] * 3 + a[i ^ 1] + a[7 - i] * 5 + p[i ^ 6] * 7 +  \
               total_##N(a, 8);                                         \
    }                                                                   \
    long indexed_value_##N(T v, int i)                                  \
    {                                                                   \
        T a[8];                                                         \
        long r = fill_##N(a, 8, v);                                     \
        long x = a[i]++;                                                \
        long y = a[i ^ 1] += v;                                         \
        long z = --a[i ^ 2];                                            \
        long w = a[i ^ 3] = v * 3;                                      \
        long u = a[i ^ 4]--;                                            \
        long t = ++a[i ^ 5];                                            \
        long s = a[i ^ 6] <<= 2;                                        \
                                                                        \
        return r + x * 3 + y * 5 + z * 7 + w * 11 + u * 13 + t * 17 +   \
               s * 19 + total_##N(a, 8);                                \
    }                                                                   \
    long indexed_held_##N(T v, int i)                                   \
    {                                                                   \
        T a[8];                                                         \
        register int k = i ^ 7;                                         \
        long r = fill_##N(a, 8, v);                                     \
                                                                        \
        a[turn(i)] = v;                                                 \
        a[(i + turn(i)) & 7] += v;                                      \
        a[i ^ 2] += mix1(v);                                            \
        i[a] -= 2;                                                      \
        a[k] = a[k] * 3 + v;                                            \
        a[k]++;                                                         \
        return r + v * 5 + a[turn(k)] * 3 + (i ^ 4)[a] + a[k] * 5 +     \
               mix1(a[i]) + total_##N(a, 8);                            \
    }                                                                   \
    long indexed_matrix_##N(long v, int i)                              \
    {                                                                   \
        T m[3][5];                                                      \
        int c = i % 5;                                                  \
        int j = i % 3;                                                  \
        long r = fill_rows_##N(m, 3, v);                                \
                                                                        \
        m[1][c] += 2;                                                   \
        m[2][c] = v;                                                    \
        m[0][c]++;                                                      \
        m[j][c] ^= 3;                                                   \
        m[j][4] -= 1;                                                   \
        return r + m[1][c] * 3 + m[j][2] * 5 + m[j][c] +                \
               *(m[2] + c) * 7 + (c - 1)[m[1]] + total_rows_##N(m, 3);  \
    }                                                                   \
    long indexed_distance_##N(int i)                                    \
    {                                                                   \
        T a[8];                                                         \
        T m[3][5];                                                      \
                                                                        \
        return (long) &a[i] - (long) a +                                \
               ((long) (a + i) - (long) a) * 100 +                      \
               ((long) (i + a) - (long) a) * 10000 +                    \
               ((long) &m[1][i % 5] - (long) m) * 1000000 +             \
               ((long) m[i % 3] - (long) m) * 100000000 +               \
               ((long) (a - i) - (long) a) * 10000000000 +              \
               (&a[i] - &a[1]) * 1000000000000;                         \
    }

TYPES(INDEXED)

/* an array of pointers by a variable index: an element written, read,
   stepped in place and dereferenced, and subscripted by a constant in
   its turn; q8 points 8 bytes past p, and i is 0 to 3 */
long indexed_pointers(long *p, long *q8, long v, int i)
{
    long *q[4];

    q[i] = p;
    q[i ^ 1] = q8;
    q[i ^ 2] = q[i];
    q[i ^ 3] = 0;
    *q[i ^ 1] = v;
    q[i ^ 2]++;
    *q[i ^ 2] += 5;

    return *q[i] + q[i ^ 1][0] * 3 + q[i][1] * 5 + (q[i ^ 3] == 0) * 7 +
           ((long) &q[i] - (long) q) * 11;
}

/* A constant over an lvalue's address that is no name's, backend.md
   6.3: the constant in the displacement of that address, and no
   instruction forming the intermediate address.  Rows of 8 bytes,
   whose size is a scale, of a local array by a variable index, m[i][c],
   the slot the base's part on x86-64 (6.3, a slot and an index), and
   the same rows through a parameter, a pointer to them, p[i][c],
   p[i][-1], (*p)[c] and p[2][c]; rows of 3 elements by a constant, the
   two constants one, q[1][2] and q[2][-1], and their addresses as
   values, &q[1][1], q[2] - 2 and q[1] - 3; read and written, operated
   on in place for the effect and for the value, ++ and -- either side;
   by an index with a call in it and by a register variable; and rows
   of 16 bytes, no scale, whose address is formed before the constant.
   The addresses as their distance in bytes from the array's start.  i
   is 0 to 3. */
#define ROW(T) (8 / (int) sizeof(T))
#define SUMMED(T, N)                                                    \
    long rows_##N(T (*p)[ROW(T)], int i, T v)                           \
    {                                                                   \
        register int k = i ^ 3;                                         \
                                                                        \
        p[i][0] = v;                                                    \
        p[i ^ 1][ROW(T) - 1] += v;                                      \
        p[i ^ 2][ROW(T) - 1]++;                                         \
        --p[k][0];                                                      \
        p[turn(i) & 3][ROW(T) - 1] ^= 0x55;                             \
        (*p)[ROW(T) - 1] -= 1;                                          \
        p[i | 1][-1] <<= 2;                                             \
        return p[i][ROW(T) - 1] * 3 + *p[i ^ 2] + p[i | 1][-1] * 5 +    \
               (p[i] + ROW(T) - 1)[0] * 7 + p[k][0] * 11 + p[2][0];     \
    }                                                                   \
    long rows_value_##N(T (*p)[ROW(T)], int i, T v)                     \
    {                                                                   \
        long x = p[i][ROW(T) - 1]++;                                    \
        long y = p[i ^ 1][0] += v;                                      \
        long z = --p[i ^ 2][ROW(T) - 1];                                \
        long w = p[i ^ 3][0] = v * 3;                                   \
        long u = p[2][ROW(T) - 1] -= mix1(v);                           \
                                                                        \
        return x * 3 + y * 5 + z * 7 + w * 11 + u * 13;                 \
    }                                                                   \
    long summed_##N(T v, int i)                                         \
    {                                                                   \
        T m[4][ROW(T)];                                                 \
        T r[4][2 * ROW(T)];                                             \
        T t[4][3];                                                      \
        T (*q)[3] = t;                                                  \
        register int k = i ^ 3;                                         \
        long s = fill_##N(&m[0][0], 4 * ROW(T), v) +                    \
                 fill_##N(&r[0][0], 8 * ROW(T), v) +                    \
                 fill_##N(&t[0][0], 12, v);                             \
                                                                        \
        m[i][ROW(T) - 1] = v;                                           \
        m[i ^ 1][0] += v;                                               \
        m[k][ROW(T) - 1]++;                                             \
        m[turn(i) & 3][0] -= 2;                                         \
        r[i][1] ^= 3;                                                   \
        q[1][2] += v;                                                   \
        q[2][-1] = 0;                                                   \
        q[i][1]++;                                                      \
        s += rows_##N(m, i, v) + rows_value_##N(m, i, v) * 3;           \
        return s + m[i][ROW(T) - 1] * 3 + m[k][0] + r[i][1] * 5 +       \
               q[1][2] * 7 + q[2][-1] + total_##N(&q[1][1], 2) * 11 +   \
               total_##N(q[2] - 2, 3) * 13 +                            \
               total_##N(&m[0][0], 4 * ROW(T)) +                        \
               total_##N(&r[0][0], 8 * ROW(T)) +                        \
               total_##N(&t[0][0], 12);                                 \
    }                                                                   \
    long summed_distance_##N(int i)                                     \
    {                                                                   \
        T m[4][ROW(T)];                                                 \
        T t[4][3];                                                      \
        T (*p)[ROW(T)] = m;                                             \
        T (*q)[3] = t;                                                  \
                                                                        \
        return (long) &m[i][ROW(T) - 1] - (long) m +                    \
               ((long) (m[i] + 1) - (long) m) * 100 +                   \
               ((long) &p[i][ROW(T) - 1] - (long) m) * 10000 +          \
               ((long) (p[i] - 1) - (long) m) * 1000000 +               \
               ((long) &q[1][2] - (long) t) * 100000000 +               \
               ((long) (q[2] + 1) - (long) t) * 10000000000 +           \
               ((long) (q[2] - 4) - (long) t) * 1000000000000 +         \
               ((long) (q[1] - 3) - (long) t) * 10000000000000 +        \
               ((long) &p[2][ROW(T) - 1] - (long) m) * 100000000000000; \
    }

TYPES(SUMMED)

/* rows of one pointer by a variable index, through the array and
   through a pointer to its rows: an element written, read, stepped in
   place and dereferenced, and subscripted in its turn; q8 points 8
   bytes past p, and i is 0 to 3 */
long summed_pointers(long *p, long *q8, long v, int i)
{
    long *r[4][1];
    long *(*rp)[1] = r;

    r[i][0] = p;
    rp[i ^ 1][0] = q8;
    r[i ^ 2][0] = rp[i][0];
    *r[i ^ 1][0] = v;
    rp[i ^ 2][0]++;
    *rp[i ^ 2][0] += 5;

    return *r[i][0] + rp[i ^ 1][0][0] * 3 + r[i][0][1] * 5 +
           ((long) &rp[i][0] - (long) r) * 7;
}

/* An object of each type read under a cast to long and to unsigned
   long, backend.md 6.3's (T) a: a signed object's load extends to 64
   bits, and an unsigned one's is valid to 64 bits as it is.  From a
   parameter's slot, a local's, a local's beyond the reach of an
   aarch64 load (5.3), a global the file defines and one it only
   declares, through a pointer, and an element by a constant and by a
   variable index of an array through a pointer, of a local array and
   of a global one.  The result is the widened value whole.  p points
   into the main's buffer, and i is -3 to 3 through it and 0 to 3 into
   an array. */
#define WIDENED_GLOBALS(T, N)                                           \
    T defined_##N;                                                      \
    T row_##N[4];

TYPES(WIDENED_GLOBALS)

#define WIDEN(T, N, W, M)                                               \
    W wparam_##N##_##M(T a) { return (W) a; }                           \
    W wlocal_##N##_##M(T a)                                             \
    {                                                                   \
        T x = a;                                                        \
        return (W) x;                                                   \
    }                                                                   \
    W wfar_##N##_##M(T a)                                               \
    {                                                                   \
        char pad[70000];                                                \
        T x = a;                                                        \
        return (W) x;                                                   \
    }                                                                   \
    W wdefined_##N##_##M(T a)                                           \
    {                                                                   \
        defined_##N = a;                                                \
        return (W) defined_##N;                                         \
    }                                                                   \
    W wdeclared_##N##_##M(T a)                                          \
    {                                                                   \
        declared_##N = a;                                               \
        return (W) declared_##N;                                        \
    }                                                                   \
    W wderef_##N##_##M(T *p) { return (W) *p; }                         \
    W wpconst_##N##_##M(T *p) { return (W) p[3]; }                      \
    W wpvar_##N##_##M(T *p, int i) { return (W) p[i]; }                 \
    W waconst_##N##_##M(T a)                                            \
    {                                                                   \
        T x[4];                                                         \
        x[2] = a;                                                       \
        return (W) x[2];                                                \
    }                                                                   \
    W wavar_##N##_##M(T a, int i)                                       \
    {                                                                   \
        T x[4];                                                         \
        x[i] = a;                                                       \
        return (W) x[i];                                                \
    }                                                                   \
    W wgconst_##N##_##M(T a)                                            \
    {                                                                   \
        row_##N[2] = a;                                                 \
        return (W) row_##N[2];                                          \
    }                                                                   \
    W wgvar_##N##_##M(T a, int i)                                       \
    {                                                                   \
        row_##N[i] = a;                                                 \
        return (W) row_##N[i];                                          \
    }
#define WIDEN_ALL(T, N) WIDENINGS(WIDEN, T, N)

TYPES(WIDEN_ALL)

/* A subscript's index of each type, widened to 64 bits by the parser's
   cast, read from the parameter's slot and through a pointer, and
   holding a negative value: p points into the middle of the main's
   buffer, and i is -8 to 7 where its type holds it. */
#define WIDENED_INDEX(T, N)                                             \
    long windex_##N(long *p, T i) { return p[i]; }                      \
    long windex_deref_##N(long *p, T i)                                 \
    {                                                                   \
        T *q = &i;                                                      \
        return p[*q];                                                   \
    }

TYPES(WIDENED_INDEX)

/* An operator whose right operand is an object in memory reached by
   other means than a local's or a parameter's name, the instruction's
   own operand on x86-64, backend.md 6.1: a global the file only
   declares, od_; *p, op_, which holds the main's x at B + at; p[i],
   oi_, and a local array's x[i], oa_, each written with b first, i 0
   to 3.  A char or a short is promoted and keeps its load. */
#define OPERAND(T, N, name, op)                                         \
    long od_##name##_##N(T a, T b)                                      \
    {                                                                   \
        declared_##N = b;                                               \
        return a op declared_##N;                                       \
    }                                                                   \
    long op_##name##_##N(T *p, T a) { return a op *p; }                 \
    long oi_##name##_##N(T *p, T a, T b, int i)                         \
    {                                                                   \
        p[i] = b;                                                       \
        return a op p[i];                                               \
    }                                                                   \
    long oa_##name##_##N(T a, T b, int i)                               \
    {                                                                   \
        T x[4];                                                         \
        x[i] = b;                                                       \
        return a op x[i];                                               \
    }
#define OPERAND_ALL(T, N) MEMORY_OPERATORS(OPERAND, T, N)

TYPES(OPERAND_ALL)

/* *p as the right operand where p points at the left operand's object,
   which the left operand wrote: the instruction reads what the write
   left, after it (G13); through the main's pointer and to a local */
#define ALIASED(T, N)                                                   \
    long alias_##N(T *p, T b) { return (*p += b) + *p; }                \
    long alias_local_##N(T a, T b)                                      \
    {                                                                   \
        T x = a;                                                        \
        T *q = &x;                                                      \
        return (x = b) * 3 - *q;                                        \
    }

TYPES(ALIASED)

/* A division by an object in memory, *p, with values held in rax and
   rdx when it comes, and by one whose address is in them: *p and p[i]
   with p loaded into rdx, and p[i] on a pointer a call returned in rax.
   On x86-64, the values and the address's registers move out of the
   two registers the division pins first, backend.md 6.5.  p points at
   the main's x at B + at, the divisor, and i is 0. */
#define BUSY(T, N)                                                      \
    long busy_div_##N(T *p, T a, T b, T c, T d)                         \
    {                                                                   \
        return a + (b + (c + d / *p));                                  \
    }                                                                   \
    long busy_rem_##N(T *p, T a, T b, T c, T d)                         \
    {                                                                   \
        return a + (b + (c + d % *p));                                  \
    }                                                                   \
    long rdx_div_##N(T *p, T a, T b) { return a + b / *p; }             \
    long rdx_rem_##N(T *p, T a, T b) { return a + b % *p; }             \
    long rdx_index_div_##N(T *p, T a, T b, int i) { return a + b / p[i]; } \
    long rdx_index_rem_##N(T *p, T a, T b, int i) { return a + b % p[i]; } \
    long rax_div_##N(T *p, T a, int i) { return a / same_##N(p)[i]; }   \
    long rax_rem_##N(T *p, T a, int i) { return a % same_##N(p)[i]; }

TYPES(BUSY)

/* The pinned instructions of x86-64, backend.md 6.5, a division and a
   shift by a variable count, each the right operand of a node, which
   holds its left operand, or an assignment its place, out of the
   registers the instruction pins.  The node is an arithmetic operator,
   pa_, the second time over the parser's cast to long; a comparison
   as a value and as a condition, pc_; a subscript through a pointer,
   ps_, and of the global array the file defines, pg_; an assignment to
   a local array's element, pl_, to an element through a pointer, pp_,
   and to the global array's, pr_.  The compound form on a local, pkl_,
   a local array's element, pkx_, an element through a pointer, pkp_,
   and the global array's, pkg_, for its effect and for its value.  The
   divisor or the count is a constant, a register variable, a local, an
   element through a pointer and an expression, pd_, and an element of
   the array a call returns, pq_.  The reservation cannot be met with
   nine values held, pf_, and does not reach a division deeper in the
   right operand, pe_, where a, b and c are in rax, rcx and rdx when
   the instruction comes.  p points at the main's x at B + at, the
   divisor or the count, i is 0 to 3, and a subscript's index is -4 to
   4 through p and 0 to 3 into the global array. */
#define PINNED(T, N, name, op, aop)                                     \
    long pa_##name##_##N(T a, T b, T c)                                 \
    {                                                                   \
        long r = a + (b op c);                                          \
        return r * 7 + ((long) a - (b op c));                           \
    }                                                                   \
    long pc_##name##_##N(T a, T b, T c)                                 \
    {                                                                   \
        long r = a < (b op c);                                          \
        if (a != (b op c))                                              \
            r += 2;                                                     \
        return r;                                                       \
    }                                                                   \
    long ps_##name##_##N(T *p, T b, T c) { return p[b op c]; }          \
    long pg_##name##_##N(T a, T b, T c)                                 \
    {                                                                   \
        row_##N[0] = a;                                                 \
        row_##N[1] = b;                                                 \
        row_##N[2] = c;                                                 \
        row_##N[3] = a ^ b;                                             \
        return row_##N[b op c];                                         \
    }                                                                   \
    long pl_##name##_##N(T a, T b, T c, int i)                          \
    {                                                                   \
        T x[4];                                                         \
        x[0] = a;                                                       \
        x[1] = a;                                                       \
        x[2] = a;                                                       \
        x[3] = a;                                                       \
        x[i] = b op c;                                                  \
        return x[0] + x[1] * 3L + x[2] * 5L + x[3] * 7L;                \
    }                                                                   \
    long pp_##name##_##N(T *p, T b, T c, int i) { return p[i] = b op c; } \
    long pr_##name##_##N(T b, T c, int i)                               \
    {                                                                   \
        row_##N[0] = b;                                                 \
        row_##N[1] = b;                                                 \
        row_##N[2] = b;                                                 \
        row_##N[3] = b;                                                 \
        row_##N[i] = b op c;                                            \
        long r = row_##N[0] + row_##N[1] * 3L;                          \
        return r + row_##N[2] * 5L + row_##N[3] * 7L;                   \
    }                                                                   \
    long pkl_##name##_##N(T a, T c)                                     \
    {                                                                   \
        T x = a;                                                        \
        x aop c;                                                        \
        long r = x aop c;                                               \
        return r * 3L + x;                                              \
    }                                                                   \
    long pkx_##name##_##N(T a, T c, int i)                              \
    {                                                                   \
        T x[4];                                                         \
        x[0] = a;                                                       \
        x[1] = a ^ 1;                                                   \
        x[2] = a ^ 2;                                                   \
        x[3] = a ^ 3;                                                   \
        x[i] aop c;                                                     \
        long r = x[i ^ 1] aop c;                                        \
        return r * 7 + x[0] + x[1] * 3L + x[2] * 5L + x[3] * 7L;        \
    }                                                                   \
    long pkp_##name##_##N(T *p, T c, int i)                             \
    {                                                                   \
        p[i] aop c;                                                     \
        return p[i ^ 1] aop c;                                          \
    }                                                                   \
    long pkg_##name##_##N(T a, T c, int i)                              \
    {                                                                   \
        row_##N[0] = a;                                                 \
        row_##N[1] = a ^ 1;                                             \
        row_##N[2] = a ^ 2;                                             \
        row_##N[3] = a ^ 3;                                             \
        row_##N[i] aop c;                                               \
        long r = row_##N[i ^ 1] aop c;                                  \
        r = r * 7 + row_##N[0] + row_##N[1] * 3L;                       \
        return r + row_##N[2] * 5L + row_##N[3] * 7L;                   \
    }                                                                   \
    long pd_##name##_##N(T *p, T a, T b, T c, int i)                    \
    {                                                                   \
        register T r = c;                                               \
        T y = c;                                                        \
        long s = a + (b op 3);                                          \
        s = s * 7 + (a + (b op r));                                     \
        s = s * 7 + (a + (b op y));                                     \
        s = s * 7 + (a + (b op p[i]));                                  \
        return s * 7 + (a + (b op (c + 0)));                            \
    }                                                                   \
    long pq_##name##_##N(T *p, T a, int i) { return a op same_##N(p)[i]; } \
    long pf_##name##_##N(T a, T b, T c, T d, T e, T f, T g, T x, T y,   \
                         T z)                                           \
    {                                                                   \
        return (a + (b + (c + (d + (e + (f + (g + (x + (y op z)))))))))  \
               + (y op z);                                              \
    }                                                                   \
    long pe_##name##_##N(T a, T b, T c, T d, T e)                       \
    {                                                                   \
        return a + (b + (c + ((d op e) + 1)));                          \
    }
#define PINNED_ALL(T, N) PINNED_OPERATORS(PINNED, T, N)

PINNED_TYPES(PINNED_ALL)

/* A dividend that arrives in rdx on x86-64: rax and rcx hold values
   when it is loaded, so it takes rdx, and the division moves it to
   rax, where the quotient stays, backend.md 6.5.  A pointer difference
   after a held product and a held sum, rd_diff_; a dividend beside a
   quotient held in rax and its sibling held in rcx, rd_div_, and the
   same for a remainder, which stays in rdx, rd_rem_.  p is q plus 0
   to 3 elements in the main's buffer. */
#define DIVIDEND(T, N)                                                  \
    long rd_diff_##N(T *p, T *q, long a, long b)                        \
    {                                                                   \
        return a * 3 + (b + (p - q));                                   \
    }                                                                   \
    long rd_div_##N(T a, T b, T c, T d, T e) { return a / b + (c + d / e); } \
    long rd_rem_##N(T a, T b, T c, T d, T e) { return a / b + (c + d % e); }

WIDE(DIVIDEND)

/* A condition on an object in memory, on x86-64 the cmp or test of the
   object itself, backend.md 7.1: compared with a constant on either
   side, kt_ and ktl_, through the main's pointer, and as a global the
   file only declares, ktd_; tested alone, under !, and with & a
   constant, under == 0 too, in tests_.  A char or a short is promoted
   by the comparison and by &, and keeps its load there. */
#define KTEST(T, N, name, op, constant)                                 \
    int kt_##name##_##N(T *p)                                           \
    {                                                                   \
        if (*p op constant)                                             \
            return 1;                                                   \
        return 0;                                                       \
    }                                                                   \
    int ktl_##name##_##N(T *p)                                          \
    {                                                                   \
        if (constant op *p)                                             \
            return 1;                                                   \
        return 0;                                                       \
    }                                                                   \
    int ktd_##name##_##N(T a)                                           \
    {                                                                   \
        declared_##N = a;                                               \
        if (declared_##N op constant)                                   \
            return 1;                                                   \
        return 0;                                                       \
    }
#define KTEST_ALL(T, N) KTESTS(KTEST, T, N)

TYPES(KTEST_ALL)

#define TESTS(T, N)                                                     \
    long tests_##N(T *p, T a)                                           \
    {                                                                   \
        long r = 0;                                                     \
        declared_##N = a;                                               \
        if (*p)                                                         \
            r += 1;                                                     \
        if (!*p)                                                        \
            r += 2;                                                     \
        if (*p & 1)                                                     \
            r += 4;                                                     \
        if ((*p & 0x81) == 0)                                           \
            r += 8;                                                     \
        if (declared_##N)                                               \
            r += 16;                                                    \
        if (declared_##N & 0x10)                                        \
            r += 32;                                                    \
        if (*p && declared_##N)                                         \
            r += 64;                                                    \
        return r;                                                       \
    }

TYPES(TESTS)
