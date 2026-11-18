/* The globals probe: objects at file scope, in rcc's C.  For each
   integer type, the objects this file defines, d_N, da_N, dm_N, dp_N
   and dq_N, and those it only declares, x_N, xa_N, xm_N, xp_N and
   xq_N, which globals-lib.c defines; each read, written, operated on
   in memory and for its value, subscripted by a constant and by a
   variable, its address taken and passed, and a pointer dereferenced
   and stepped.  The rows are backend.md 6.3's and 6.6's and
   aarch64.md 6.3's for a global, and the two of chapter 10,
   PC-relative and through the GOT; and 6.3's address by a name and a
   constant, an element by a constant index folded into the label's
   operand, on a static array that crosses a page too.  See
   globals.h. */

#include "globals.h"

/* the objects: those of d defined here, tentatively, and those of x
   declared, the arrays without their size */
#define OBJECTS(T, N)               \
        T d_##N;                    \
        T da_##N[ARRAY];            \
        T dm_##N[ROWS][COLUMNS];    \
        T *dp_##N;                  \
        T *dq_##N[CELLS];           \
        extern T x_##N;             \
        extern T xa_##N[];          \
        extern T xm_##N[][COLUMNS]; \
        extern T *xp_##N;           \
        extern T *xq_##N[];

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

int turn(int i);

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

/* An element by a constant, its offset folded into the label's operand
   where the file defines the object and the offset lies within it,
   backend.md 6.3, and the GOT's rows where it only declares it: an
   element of the array of arrays by a negative column and by one past
   a row's end, both inside the array; a constant and zero stored, the
   memory forms and the shifts in place, ++ and -- for the effect and
   for the value; through the sum of the array and a constant, of a
   constant and the array, of an element's address and a constant, and
   their difference from a constant, and a row of the array of arrays
   so reached; the constant 0, *a, *m[1] and **m; the addresses of
   these forms returned, and as their distance in bytes from the
   array's start; and addresses outside the object, below its start
   and past its end, which take the rows of p + n, beside one exactly
   one past the end, which folds. */
#define FOLDS(T, N, K)                                                       \
        long fold_before_##K##_##N(void)                                     \
        {                                                                    \
                return K##m_##N[1][-1];                                      \
        }                                                                    \
        long fold_beyond_##K##_##N(void)                                     \
        {                                                                    \
                return K##m_##N[1][COLUMNS];                                 \
        }                                                                    \
        void fold_put_##K##_##N(long v)                                      \
        {                                                                    \
                K##m_##N[0][COLUMNS + 1] = v;                                \
                K##m_##N[2][-2] = 0;                                         \
                K##a_##N[4] = 0x12345;                                       \
        }                                                                    \
        void fold_bump_##K##_##N(long v)                                     \
        {                                                                    \
                K##a_##N[2] += v;                                            \
                K##a_##N[7] ^= 0x55;                                         \
                K##m_##N[2][1] -= v;                                         \
        }                                                                    \
        void fold_shift_##K##_##N(int n)                                     \
        {                                                                    \
                K##a_##N[6] <<= n;                                           \
                K##m_##N[1][3] >>= n;                                        \
        }                                                                    \
        void fold_tick_##K##_##N(void)                                       \
        {                                                                    \
                K##a_##N[1]++;                                               \
                --K##m_##N[2][2];                                            \
        }                                                                    \
        long fold_tick_value_##K##_##N(void)                                 \
        {                                                                    \
                long old = K##a_##N[4]++;                                    \
                                                                             \
                return old * 3 + --K##a_##N[0];                              \
        }                                                                    \
        long fold_bump_value_##K##_##N(long v)                               \
        {                                                                    \
                return K##a_##N[7] -= v;                                     \
        }                                                                    \
        long fold_sum_##K##_##N(void)                                        \
        {                                                                    \
                return *(K##a_##N + 5) + (K##a_##N + 2)[4] * 3 +             \
                       *(3 + K##a_##N) * 5;                                  \
        }                                                                    \
        long fold_minus_##K##_##N(void)                                      \
        {                                                                    \
                return *(K##a_##N + 7 - 2) + (&K##a_##N[6])[-4] * 3 +        \
                       *(K##a_##N - -1) * 5;                                 \
        }                                                                    \
        long fold_nested_##K##_##N(void)                                     \
        {                                                                    \
                return (&K##m_##N[1][0])[2] + *(K##m_##N[2] + 1) * 3 +       \
                       (*(K##m_##N + 1))[3] * 5;                             \
        }                                                                    \
        T *fold_third_##K##_##N(void)                                        \
        {                                                                    \
                return &K##a_##N[3];                                         \
        }                                                                    \
        T *fold_plus_##K##_##N(void)                                         \
        {                                                                    \
                return 6 + K##a_##N;                                         \
        }                                                                    \
        T *fold_back_##K##_##N(void)                                         \
        {                                                                    \
                return &K##m_##N[2][-1];                                     \
        }                                                                    \
        T *fold_row_##K##_##N(void)                                          \
        {                                                                    \
                return K##m_##N[2];                                          \
        }                                                                    \
        long fold_zero_##K##_##N(long v)                                     \
        {                                                                    \
                *K##a_##N += v;                                              \
                *K##m_##N[1] = v;                                            \
                return *K##a_##N + *K##m_##N[2] * 3 + **K##m_##N * 5;        \
        }                                                                    \
        T *fold_below_##K##_##N(void)                                        \
        {                                                                    \
                return &K##a_##N[-2];                                        \
        }                                                                    \
        T *fold_past_##K##_##N(void)                                         \
        {                                                                    \
                return K##a_##N + ARRAY + 3;                                 \
        }                                                                    \
        T *fold_end_##K##_##N(void)                                          \
        {                                                                    \
                return K##a_##N + ARRAY;                                     \
        }                                                                    \
        T *fold_row_past_##K##_##N(void)                                     \
        {                                                                    \
                return &K##m_##N[ROWS][1];                                   \
        }                                                                    \
        long fold_distance_##K##_##N(void)                                   \
        {                                                                    \
                return (long) &K##a_##N[5] - (long) K##a_##N +               \
                       ((long) (K##m_##N[2] + 1) - (long) K##m_##N) * 1000 + \
                       ((long) (K##a_##N + ARRAY) - (long) &K##a_##N[-1]) *  \
                           1000000;                                          \
        }

/* The array of pointers: an element by a constant, the pointer it
   holds, read, written, stepped in place and dereferenced, and
   subscripted by a constant in its turn. */
#define CELLS_OF(T, N, K)                 \
        T *cell_get_##K##_##N(void)       \
        {                                 \
                return K##q_##N[0];       \
        }                                 \
        void cell_put_##K##_##N(T *p)     \
        {                                 \
                K##q_##N[3] = p;          \
        }                                 \
        long cell_deref_##K##_##N(void)   \
        {                                 \
                return *K##q_##N[2];      \
        }                                 \
        long cell_index_##K##_##N(void)   \
        {                                 \
                return K##q_##N[1][1];    \
        }                                 \
        void cell_step_##K##_##N(void)    \
        {                                 \
                K##q_##N[1]++;            \
                K##q_##N[2] -= 1;         \
        }                                 \
        void cell_store_##K##_##N(long v) \
        {                                 \
                *K##q_##N[3] = v;         \
        }

/* An element by a variable index, which keeps the array's address
   formed on both kinds, since a PC-relative operand takes no index
   (backend.md 6.3): read and written, operated on in place for the
   effect and for the value, ++ and -- either side; by i[a], by an index
   with a call in it and by a register variable; an element of a row by
   a constant, m[1][i], and of a row by a variable; an element of the
   array of pointers, the pointer stepped and dereferenced; and a - i,
   with the other addresses as their distance in bytes from the
   array's start.  i is 0 to 7. */
#define INDEXED(T, N, K)                                                     \
        void indexed_##K##_##N(long v, int i)                                \
        {                                                                    \
                register int k = i ^ 7;                                      \
                                                                             \
                K##a_##N[i ^ 1] += v;                                        \
                K##a_##N[i ^ 2]++;                                           \
                --K##a_##N[i ^ 3];                                           \
                K##a_##N[turn(i)] ^= 0x55;                                   \
                K##a_##N[k] = K##a_##N[k] * 3 + v;                           \
                (i ^ 4)[K##a_##N] = 0;                                       \
                K##m_##N[1][i & 3] -= v;                                     \
                K##m_##N[i % 3][i & 3] <<= 2;                                \
        }                                                                    \
        long indexed_value_##K##_##N(long v, int i)                          \
        {                                                                    \
                long x = K##a_##N[i]++;                                      \
                long y = K##a_##N[i ^ 1] += v;                               \
                long z = --K##a_##N[i ^ 2];                                  \
                long w = K##a_##N[i ^ 3] = v * 3;                            \
                long u = K##m_##N[2][i & 3]--;                               \
                                                                             \
                return x * 3 + y * 5 + z * 7 + w * 11 + u * 13 +             \
                       K##a_##N[turn(i)] * 17 + K##m_##N[i % 3][i & 3] * 19; \
        }                                                                    \
        long indexed_cells_##K##_##N(long v, int i)                          \
        {                                                                    \
                int c = i & 3;                                               \
                                                                             \
                *K##q_##N[c] += v;                                           \
                K##q_##N[c ^ 1]++;                                           \
                return *K##q_##N[c] + *K##q_##N[c ^ 1] * 3 +                 \
                       K##q_##N[c ^ 2][1] * 5;                               \
        }                                                                    \
        long indexed_distance_##K##_##N(int i)                               \
        {                                                                    \
                return (long) &K##a_##N[i] - (long) K##a_##N +               \
                       ((long) (K##a_##N + i) - (long) K##a_##N) * 100 +     \
                       ((long) (i + K##a_##N) - (long) K##a_##N) * 10000 +   \
                       ((long) (K##a_##N - i) - (long) K##a_##N) * 1000000 + \
                       ((long) &K##m_##N[1][i & 3] - (long) K##m_##N) *      \
                           100000000 +                                       \
                       ((long) K##m_##N[i % 3] - (long) K##m_##N) *          \
                           10000000000;                                      \
        }

/* A constant over the address of a row by a variable index, the
   constant in that address's displacement, backend.md 6.3: on the
   array of arrays, whose rows of 4 are a scale for a char and a short
   and none for an int and a long, and through a pointer to its rows;
   read and written, operated on in place for the effect and for the
   value, ++ and -- either side, by a register variable and with a call
   in the right operand; a column of -1; the addresses of elements of a
   row by a constant as values, passed to the library; and the
   addresses as their distance in bytes from the array's start.  The rows of the object
   the file only declares sum the constants of the GOT's address, as
   corner and origin do.  i is 0 to 7. */
#define SUMMED(T, N, K)                                                     \
        void summed_##K##_##N(long v, int i)                                \
        {                                                                   \
                int r = i % ROWS;                                           \
                register int k = (r + 1) % ROWS;                            \
                T(*p)[COLUMNS] = K##m_##N;                                  \
                                                                            \
                K##m_##N[r][2] += v;                                        \
                K##m_##N[k][3]++;                                           \
                --K##m_##N[r][0];                                           \
                K##m_##N[r + 1][-1] ^= 0x55;                                \
                p[k][1] = v;                                                \
                p[r][3] <<= 2;                                              \
                (*p)[2] -= v;                                               \
        }                                                                   \
        long summed_value_##K##_##N(long v, int i)                          \
        {                                                                   \
                int r = i % ROWS;                                           \
                T(*p)[COLUMNS] = K##m_##N;                                  \
                long x = K##m_##N[r][2]++;                                  \
                long y = K##m_##N[(r + 1) % ROWS][1] += v;                  \
                long z = --p[r][3];                                         \
                long w = p[(r + 2) % ROWS][0] = v * 3;                      \
                long u = K##m_##N[r][1] -= turn(i);                         \
                long t = K##m_##N[(r + 2) % ROWS][3] = turn(i + 1);         \
                long q = p[r][2] = v / (i + 1);                             \
                long e = p[r][1] = 1 + v % (i + 2);                         \
                                                                            \
                return x * 3 + y * 5 + z * 7 + w * 11 + u * 13 + t * 37 +   \
                       q * 41 + e * 43 +                                    \
                       K##m_##N[r + 1][-1] * 17 + p[r][2] * 19 +            \
                       total_##N(&K##m_##N[1][1], 2) * 23 +                 \
                       total_##N(&p[1][1], 2) * 29 +                        \
                       total_##N(p[2] - 1, 2) * 31;                         \
        }                                                                   \
        long summed_distance_##K##_##N(int i)                               \
        {                                                                   \
                int r = i % ROWS;                                           \
                T(*p)[COLUMNS] = K##m_##N;                                  \
                                                                            \
                return (long) &K##m_##N[r][2] - (long) K##m_##N +           \
                       ((long) (K##m_##N[r] + 3) - (long) K##m_##N) * 100 + \
                       ((long) &p[r][1] - (long) K##m_##N) * 10000 +        \
                       ((long) (p[r] - 1) - (long) K##m_##N) * 1000000;     \
        }

#define KIND(T, N, K) \
        SCALAR(T, N, K) ARRAYS(T, N, K) ADDRESSES(T, N, K) POINTERS(T, N, K)
#define FOLD_KIND(T, N, K) \
        FOLDS(T, N, K) CELLS_OF(T, N, K) INDEXED(T, N, K) SUMMED(T, N, K)
#define BOTH_KINDS(T, N)   KINDS(KIND, T, N) KINDS(FOLD_KIND, T, N)

TYPES(BOTH_KINDS)

/* A static array that crosses a page: an element on each side of the
   boundary 4096 bytes in, and the last, read and written by a constant
   index, the address of the one past the boundary returned, and the
   start given to the main; and addresses at its end and outside it.  On aarch64 the page is the element's,
   adrp x16, big+4096, not the array's. */
static char big[BIG];

char *
big_base(void)
{
        return big;
}

long
big_read(void)
{
        return big[4095] + big[4096] * 3 + big[BIG - 1] * 5 + big[0] * 7;
}

void
big_write(long v)
{
        big[4095] = v;
        big[4096] = v >> 8;
        big[BIG - 1] = v >> 16;
}

char *
big_middle(void)
{
        return &big[4096];
}

/* one past its end, which folds, and beyond it and below its start,
   which take the rows of p + n */
char *
big_end(void)
{
        return big + BIG;
}

char *
big_beyond(void)
{
        return big + BIG + 4096;
}

char *
big_below(void)
{
        return &big[-4096];
}

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
