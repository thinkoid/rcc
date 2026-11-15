/* The globals probe: objects at file scope, in rcc's C.  For each
   integer type, the objects this file defines, d_N, da_N, dm_N and
   dp_N, and those it only declares, x_N, xa_N, xm_N and xp_N, which
   globals-lib.c defines; each read, written, operated on in memory and
   for its value, subscripted by a constant and by a variable, its
   address taken and passed, and a pointer dereferenced and stepped.
   The rows are backend.md 6.3's and 6.6's and aarch64.md 6.3's for a
   global, and the two of chapter 10, PC-relative and through the GOT.
   See globals.h. */

#include "globals.h"

/* the objects: those of d defined here, tentatively, and those of x
   declared, the arrays without their size */
#define OBJECTS(T, N)               \
        T d_##N;                    \
        T da_##N[ARRAY];            \
        T dm_##N[ROWS][COLUMNS];    \
        T *dp_##N;                  \
        extern T x_##N;             \
        extern T xa_##N[];          \
        extern T xm_##N[][COLUMNS]; \
        extern T *xp_##N;

TYPES(OBJECTS)

/* globals-lib.c: touch_N reads the object a pointer points to and
   writes it, fill_N writes n objects and sums what they held, total_N
   sums n objects by weight; poke_K_N writes K_N, and so writes an
   object behind the back of the code that reads it */
#define LIBRARY(T, N)                       \
        long touch_##N(T *p, long v);       \
        long fill_##N(T *a, int n, long v); \
        long total_##N(T *a, int n);        \
        void poke_d_##N(long v);            \
        void poke_x_##N(long v);

TYPES(LIBRARY)

/* The object itself, 6.3's load and store rows and 6.6's memory forms:
   read, written, a constant and zero stored, each compound assignment
   that has a memory form and one that has none, as a statement and for
   its value; ++ and -- either side, for the effect and for the value;
   read twice in one expression and written by it; read before and
   after a call that writes it. */
#define SCALAR(T, N, K)                       \
        long read_##K##_##N(void)             \
        {                                     \
                return K##_##N;               \
        }                                     \
        int read_int_##K##_##N(void)          \
        {                                     \
                return K##_##N;               \
        }                                     \
        void write_##K##_##N(long v)          \
        {                                     \
                K##_##N = v;                  \
        }                                     \
        long write_value_##K##_##N(long v)    \
        {                                     \
                return K##_##N = v;           \
        }                                     \
        void zero_##K##_##N(void)             \
        {                                     \
                K##_##N = 0;                  \
        }                                     \
        void constant_##K##_##N(void)         \
        {                                     \
                K##_##N = 0x12345;            \
        }                                     \
        void add_##K##_##N(long v)            \
        {                                     \
                K##_##N += v;                 \
        }                                     \
        void add_five_##K##_##N(void)         \
        {                                     \
                K##_##N += 5;                 \
        }                                     \
        long add_value_##K##_##N(long v)      \
        {                                     \
                return K##_##N += v;          \
        }                                     \
        void sub_##K##_##N(int v)             \
        {                                     \
                K##_##N -= v;                 \
        }                                     \
        void xor_##K##_##N(long v)            \
        {                                     \
                K##_##N ^= v;                 \
        }                                     \
        void shl_##K##_##N(int n)             \
        {                                     \
                K##_##N <<= n;                \
        }                                     \
        void shr_##K##_##N(int n)             \
        {                                     \
                K##_##N >>= n;                \
        }                                     \
        long shr_value_##K##_##N(int n)       \
        {                                     \
                return K##_##N >>= n;         \
        }                                     \
        void mul_##K##_##N(long v)            \
        {                                     \
                K##_##N *= v;                 \
        }                                     \
        long div_value_##K##_##N(int v)       \
        {                                     \
                return K##_##N /= v;          \
        }                                     \
        void inc_##K##_##N(void)              \
        {                                     \
                K##_##N++;                    \
        }                                     \
        void dec_##K##_##N(void)              \
        {                                     \
                --K##_##N;                    \
        }                                     \
        long postinc_##K##_##N(void)          \
        {                                     \
                return K##_##N++;             \
        }                                     \
        long preinc_##K##_##N(void)           \
        {                                     \
                return ++K##_##N;             \
        }                                     \
        long postdec_##K##_##N(void)          \
        {                                     \
                return K##_##N--;             \
        }                                     \
        long again_##K##_##N(void)            \
        {                                     \
                K##_##N = K##_##N + 1;        \
                return K##_##N * 3 - K##_##N; \
        }                                     \
        long around_##K##_##N(long v)         \
        {                                     \
                long before = K##_##N;        \
                poke_##K##_##N(v);            \
                return before * 7 + K##_##N;  \
        }

/* The arrays: an element by a constant and by a variable index, read,
   written and operated on in place; a row of the array of arrays by a
   constant and by a variable, an element of it, and the row decayed and
   passed. */
#define ARRAYS(T, N, K)                                 \
        long elem_##K##_##N(void)                       \
        {                                               \
                return K##a_##N[3];                     \
        }                                               \
        long elem_at_##K##_##N(int i)                   \
        {                                               \
                return K##a_##N[i];                     \
        }                                               \
        void put_##K##_##N(long v)                      \
        {                                               \
                K##a_##N[5] = v;                        \
        }                                               \
        void put_at_##K##_##N(int i, long v)            \
        {                                               \
                K##a_##N[i] = v;                        \
        }                                               \
        void bump_at_##K##_##N(int i, long v)           \
        {                                               \
                K##a_##N[i] += v;                       \
        }                                               \
        long inc_at_##K##_##N(int i)                    \
        {                                               \
                return K##a_##N[i]++;                   \
        }                                               \
        long last_##K##_##N(void)                       \
        {                                               \
                return K##a_##N[ARRAY - 1];             \
        }                                               \
        long cell_##K##_##N(int i, int j)               \
        {                                               \
                return K##m_##N[i][j];                  \
        }                                               \
        long corner_##K##_##N(void)                     \
        {                                               \
                return K##m_##N[2][3];                  \
        }                                               \
        long origin_##K##_##N(void)                     \
        {                                               \
                return K##m_##N[0][1];                  \
        }                                               \
        void set_cell_##K##_##N(int i, int j, long v)   \
        {                                               \
                K##m_##N[i][j] = v;                     \
        }                                               \
        void set_row_one_##K##_##N(int j, long v)       \
        {                                               \
                K##m_##N[1][j] = v;                     \
        }                                               \
        long row_##K##_##N(int i)                       \
        {                                               \
                return total_##N(K##m_##N[i], COLUMNS); \
        }

/* The address: of the object, of an array decayed and of an element,
   returned and passed to a function that reads and writes through it. */
#define ADDRESSES(T, N, K)                           \
        T *address_##K##_##N(void)                   \
        {                                            \
                return &K##_##N;                     \
        }                                            \
        T *array_##K##_##N(void)                     \
        {                                            \
                return K##a_##N;                     \
        }                                            \
        T *elem_address_##K##_##N(int i)             \
        {                                            \
                return &K##a_##N[i];                 \
        }                                            \
        T *row_address_##K##_##N(int i)              \
        {                                            \
                return K##m_##N[i];                  \
        }                                            \
        long pass_##K##_##N(long v)                  \
        {                                            \
                return touch_##N(&K##_##N, v);       \
        }                                            \
        long pass_array_##K##_##N(long v)            \
        {                                            \
                return fill_##N(K##a_##N, ARRAY, v); \
        }

/* The pointer: read and written as an object, dereferenced, stored
   through, subscripted and stepped, for the effect and for the value,
   and the access through the old value of a step. */
#define POINTERS(T, N, K)                    \
        T *pointer_##K##_##N(void)           \
        {                                    \
                return K##p_##N;             \
        }                                    \
        void set_pointer_##K##_##N(T *q)     \
        {                                    \
                K##p_##N = q;                \
        }                                    \
        long deref_##K##_##N(void)           \
        {                                    \
                return *K##p_##N;            \
        }                                    \
        void store_through_##K##_##N(long v) \
        {                                    \
                *K##p_##N = v;               \
        }                                    \
        long index_##K##_##N(int i)          \
        {                                    \
                return K##p_##N[i];          \
        }                                    \
        void step_##K##_##N(void)            \
        {                                    \
                K##p_##N++;                  \
        }                                    \
        void step_by_##K##_##N(int n)        \
        {                                    \
                K##p_##N += n;               \
        }                                    \
        void back_##K##_##N(void)            \
        {                                    \
                K##p_##N -= 2;               \
        }                                    \
        T *step_value_##K##_##N(void)        \
        {                                    \
                return K##p_##N++;           \
        }                                    \
        T *step_by_value_##K##_##N(int n)    \
        {                                    \
                return K##p_##N += n;        \
        }                                    \
        long walk_##K##_##N(void)            \
        {                                    \
                return *K##p_##N++;          \
        }                                    \
        void walk_store_##K##_##N(long v)    \
        {                                    \
                *K##p_##N++ = v;             \
        }

#define KIND(T, N, K) \
        SCALAR(T, N, K) ARRAYS(T, N, K) ADDRESSES(T, N, K) POINTERS(T, N, K)
#define BOTH_KINDS(T, N) KINDS(KIND, T, N)

TYPES(BOTH_KINDS)

/* The data: an object of each type with an initializer, the main
   reading each before anything writes it; null pointers. */
#define DEFINE_INITIALIZED(T, name, value) T name = value

INITIALIZED(DEFINE_INITIALIZED)

int *i_null = 0;
char *i_void_null = (void *) 0;
void (*i_handler)(int) = 0;

/* A tentative definition written twice is one object, zero, 6.9.3p2;
   extern before a tentative definition and before a definition with an
   initializer declares the same object.  late is used before its
   definition at the end of the file: a definition anywhere in the file
   makes it an object the file defines, reached PC-relative. */
long t_twice;
long t_twice;

extern int t_extern;
int t_extern;

extern int late;

long
read_twice(void)
{
        return t_twice;
}

void
write_twice(long v)
{
        t_twice = v;
}

long
read_extern(void)
{
        return t_extern;
}

long
read_late(void)
{
        return late;
}

void
bump_late(int v)
{
        late += v;
}

int late = 77;
