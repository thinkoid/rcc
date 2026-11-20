/* The globals probe's main, compiled by clang -O0: calls every function
   of globals.c as rcc compiled it and as clang did (ref_), over a table
   of values, and compares.  Prints the first forty differences, or
   every one when PROBE_ALL is set in the environment, and a summary;
   exits 0 when nothing differs, 1 otherwise.

   Each build has objects of its own.  Those globals.c defines are rcc's
   under their names and the reference's under the prefix.  Those
   globals-lib.c defines are linked twice, the second copy renamed, so
   rcc's build reaches the first through the GOT and the reference's the
   second.  The main names both copies of every object.  Before each
   check it fills both alike: every scalar holds the check's value, the
   arrays the same bytes, which change from check to check, and every
   pointer the same offset into its build's buffer.  After the check it
   compares the results, then every object byte by byte, the pointers
   as offsets from the buffer, then the buffers, so a store too wide,
   one to the wrong object, or one that reached its object's copy of
   the other build shows.  An address an object's function returns is
   compared as its offset from its build's own object.  The static
   array that crosses a page, big, is filled and compared through the
   start each build's big_base() returns.

   Before anything writes them, the initialized objects of both builds
   are checked against their initializers, which the main converts to
   the object's type itself, and the tentative ones against zero. */

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "globals.h"

static const long values[] = { 0,
                               1,
                               2,
                               3,
                               5,
                               7,
                               85,
                               100,
                               127,
                               128,
                               200,
                               255,
                               256,
                               4095,
                               4096,
                               32767,
                               32768,
                               65535,
                               65536,
                               0x12345,
                               0x7fffffff,
                               0x80000000L,
                               0xffffffffL,
                               0x100000000L,
                               0x123456789abcdef0L,
                               -1,
                               -2,
                               -7,
                               -128,
                               -129,
                               -5000,
                               -32768,
                               -32769,
                               -65536,
                               -2147483647L - 1,
                               LONG_MAX,
                               LONG_MIN };

#define COUNT ((int) (sizeof values / sizeof *values))

/* the divisors of /=: none is 0 or -1, which trap on x86-64 */
static const int divisors[] = { 3, 7, -5, 100 };

#define DIVISORS ((int) (sizeof divisors / sizeof *divisors))

/* the bytes on each side of B, where the pointers point, and the guard
   beyond them */
#define SPAN  128
#define GUARD 32

static unsigned char area[2][GUARD + 2 * SPAN + GUARD]
    __attribute__((aligned(16)));
static unsigned long seed;
static long filled;

static long total;
static long bad;
static long shown = 40;

/* declarations: each object and its reference */
#define DECLARE_OBJECTS(T, N)                                      \
        extern T d_##N, ref_d_##N, x_##N, ref_x_##N;               \
        extern T da_##N[ARRAY], ref_da_##N[ARRAY];                 \
        extern T xa_##N[ARRAY], ref_xa_##N[ARRAY];                 \
        extern T dm_##N[ROWS][COLUMNS], ref_dm_##N[ROWS][COLUMNS]; \
        extern T xm_##N[ROWS][COLUMNS], ref_xm_##N[ROWS][COLUMNS]; \
        extern T *dp_##N, *ref_dp_##N, *xp_##N, *ref_xp_##N;       \
        extern T *dq_##N[CELLS], *ref_dq_##N[CELLS];               \
        extern T *xq_##N[CELLS], *ref_xq_##N[CELLS];

TYPES(DECLARE_OBJECTS)

#define DECLARE_INITIALIZED(T, name, value) extern T name, ref_##name

INITIALIZED(DECLARE_INITIALIZED)

extern int *i_null, *ref_i_null;
extern char *i_void_null, *ref_i_void_null;
extern void (*i_handler)(int), (*ref_i_handler)(int);
extern long t_twice, ref_t_twice;
extern int t_extern, ref_t_extern;
extern int late, ref_late;

/* declarations: each function and its reference */
#define BOTH(R, f, parameters) \
        R f parameters;        \
        R ref_##f parameters;

#define DECLARE_KIND(T, N, K) \
        BOTH(long, read_##K##_##N, (void))                  \
        BOTH(int, read_int_##K##_##N, (void))                  \
        BOTH(void, write_##K##_##N, (long))                  \
        BOTH(long, write_value_##K##_##N, (long))                  \
        BOTH(void, zero_##K##_##N, (void))                  \
        BOTH(void, constant_##K##_##N, (void))                  \
        BOTH(void, add_##K##_##N, (long))                  \
        BOTH(void, add_five_##K##_##N, (void))                  \
        BOTH(long, add_value_##K##_##N, (long))                  \
        BOTH(void, sub_##K##_##N, (int))                  \
        BOTH(void, xor_##K##_##N, (long))                  \
        BOTH(void, shl_##K##_##N, (int))                  \
        BOTH(void, shr_##K##_##N, (int))                  \
        BOTH(long, shr_value_##K##_##N, (int))                  \
        BOTH(void, mul_##K##_##N, (long))                  \
        BOTH(long, div_value_##K##_##N, (int))                  \
        BOTH(void, inc_##K##_##N, (void))                  \
        BOTH(void, dec_##K##_##N, (void))                  \
        BOTH(long, postinc_##K##_##N, (void))                  \
        BOTH(long, preinc_##K##_##N, (void))                  \
        BOTH(long, postdec_##K##_##N, (void))                  \
        BOTH(long, again_##K##_##N, (void))                  \
        BOTH(long, around_##K##_##N, (long))                  \
        BOTH(long, elem_##K##_##N, (void))                  \
        BOTH(long, elem_at_##K##_##N, (int))                  \
        BOTH(void, put_##K##_##N, (long))                  \
        BOTH(void, put_at_##K##_##N, (int, long))                  \
        BOTH(void, bump_at_##K##_##N, (int, long))                  \
        BOTH(long, inc_at_##K##_##N, (int))                  \
        BOTH(long, last_##K##_##N, (void))                  \
        BOTH(long, cell_##K##_##N, (int, int))                  \
        BOTH(long, corner_##K##_##N, (void))                  \
        BOTH(long, origin_##K##_##N, (void))                  \
        BOTH(void, set_cell_##K##_##N, (int, int, long))                  \
        BOTH(void, set_row_one_##K##_##N, (int, long))                  \
        BOTH(long, row_##K##_##N, (int))                  \
        BOTH(T *, address_##K##_##N, (void))                  \
        BOTH(T *, array_##K##_##N, (void))                  \
        BOTH(T *, elem_address_##K##_##N, (int))                  \
        BOTH(T *, row_address_##K##_##N, (int))                  \
        BOTH(long, pass_##K##_##N, (long))                  \
        BOTH(long, pass_array_##K##_##N, (long))                  \
        BOTH(T *, pointer_##K##_##N, (void))                  \
        BOTH(void, set_pointer_##K##_##N, (T *))                  \
        BOTH(long, deref_##K##_##N, (void))                  \
        BOTH(void, store_through_##K##_##N, (long))                  \
        BOTH(long, index_##K##_##N, (int))                  \
        BOTH(void, step_##K##_##N, (void))                  \
        BOTH(void, step_by_##K##_##N, (int))                  \
        BOTH(void, back_##K##_##N, (void))                  \
        BOTH(T *, step_value_##K##_##N, (void))                  \
        BOTH(T *, step_by_value_##K##_##N, (int))                  \
        BOTH(long, walk_##K##_##N, (void))                  \
        BOTH(void, walk_store_##K##_##N, (long))                  \
        BOTH(long, fold_before_##K##_##N, (void))                  \
        BOTH(long, fold_beyond_##K##_##N, (void))                  \
        BOTH(void, fold_put_##K##_##N, (long))                  \
        BOTH(void, fold_bump_##K##_##N, (long))                  \
        BOTH(void, fold_shift_##K##_##N, (int))                  \
        BOTH(void, fold_tick_##K##_##N, (void))                  \
        BOTH(long, fold_tick_value_##K##_##N, (void))                  \
        BOTH(long, fold_bump_value_##K##_##N, (long))                  \
        BOTH(long, fold_sum_##K##_##N, (void))                  \
        BOTH(long, fold_minus_##K##_##N, (void))                  \
        BOTH(long, fold_nested_##K##_##N, (void))                  \
        BOTH(T *, fold_third_##K##_##N, (void))                  \
        BOTH(T *, fold_plus_##K##_##N, (void))                  \
        BOTH(T *, fold_back_##K##_##N, (void))                  \
        BOTH(T *, fold_row_##K##_##N, (void))                  \
        BOTH(long, fold_zero_##K##_##N, (long))                  \
        BOTH(T *, fold_below_##K##_##N, (void))                  \
        BOTH(T *, fold_past_##K##_##N, (void))                  \
        BOTH(T *, fold_end_##K##_##N, (void))                  \
        BOTH(T *, fold_row_past_##K##_##N, (void))                  \
        BOTH(long, fold_distance_##K##_##N, (void))                  \
        BOTH(void, indexed_##K##_##N, (long, int))                  \
        BOTH(long, indexed_value_##K##_##N, (long, int))                  \
        BOTH(long, indexed_cells_##K##_##N, (long, int))                  \
        BOTH(long, indexed_distance_##K##_##N, (int))                  \
        BOTH(void, summed_##K##_##N, (long, int))                  \
        BOTH(long, summed_value_##K##_##N, (long, int))                  \
        BOTH(long, summed_distance_##K##_##N, (int))                  \
        BOTH(T *, cell_get_##K##_##N, (void))                  \
        BOTH(void, cell_put_##K##_##N, (T *))                  \
        BOTH(long, cell_deref_##K##_##N, (void))                  \
        BOTH(long, cell_index_##K##_##N, (void))                  \
        BOTH(void, cell_step_##K##_##N, (void))                  \
        BOTH(void, cell_store_##K##_##N, (long))
#define DECLARE_KINDS(T, N) KINDS(DECLARE_KIND, T, N)

TYPES(DECLARE_KINDS)

BOTH(long, read_twice, (void))
BOTH(void, write_twice, (long))
BOTH(long, read_extern, (void))
BOTH(long, read_late, (void))
BOTH(void, bump_late, (int))
BOTH(char *, big_base, (void))
BOTH(long, big_read, (void))
BOTH(void, big_write, (long))
BOTH(char *, big_middle, (void))
BOTH(char *, big_end, (void))
BOTH(char *, big_beyond, (void))
BOTH(char *, big_below, (void))

static unsigned char *
base(int k)
{
        return area[k] + GUARD + SPAN;
}

/* a pointer as its offset from where it should point near */
static long
offset(const void *p, const void *b)
{
        if (!p)
                return LONG_MIN;

        return (long) ((unsigned long) p - (unsigned long) b);
}

/* the next byte of the check's filling, the same for both builds */
static unsigned char
next_byte(void)
{
        const unsigned long i = (unsigned long) filled++;

        return (unsigned char) (seed * 0x9e3779b1UL + i * 151UL + (i >> 3));
}

static void
fill(void *a, void *b, size_t n)
{
        unsigned char *p = a;
        unsigned char *q = b;

        for (size_t i = 0; i < n; ++i)
                p[i] = q[i] = next_byte();
}

#define PRIME_TYPE(T, N)                                                      \
        d_##N = ref_d_##N = x_##N = ref_x_##N = (T) x;                        \
        fill(da_##N, ref_da_##N, sizeof da_##N);                              \
        fill(xa_##N, ref_xa_##N, sizeof xa_##N);                              \
        fill(dm_##N, ref_dm_##N, sizeof dm_##N);                              \
        fill(xm_##N, ref_xm_##N, sizeof xm_##N);                              \
        dp_##N = xp_##N = (T *) (base(0) + at);                               \
        ref_dp_##N = ref_xp_##N = (T *) (base(1) + at);                       \
        for (int j = 0; j < CELLS; ++j) {                                     \
                dq_##N[j] = xq_##N[j] = (T *) (base(0) + at + 8 * j);         \
                ref_dq_##N[j] = ref_xq_##N[j] = (T *) (base(1) + at + 8 * j); \
        }

/* Both builds' objects and buffers filled alike, x in every scalar and
   at B + at, where every pointer points, and each array of pointers'
   elements 8 bytes apart from there. */
static void
prime(long x, int at)
{
        ++seed;
        filled = 0;

        fill(area[0], area[1], sizeof area[0]);
        memcpy(base(0) + at, &x, sizeof x);
        memcpy(base(1) + at, &x, sizeof x);

        TYPES(PRIME_TYPE)
}

static void
report(const char *name, long x, long got, long want)
{
        ++total;

        if (got == want)
                return;

        if (++bad <= shown)
                printf("%s(%ld, ...): rcc %ld, clang %ld\n", name, x, got,
                       want);
}

/* one object of both builds byte by byte, or one pointer as offsets
   from each build's buffer: whether they differ, said once */
static int
bytes_differ(const char *name,
             long x,
             const char *object,
             const void *a,
             const void *b,
             size_t n)
{
        const unsigned char *p = a;
        const unsigned char *q = b;

        for (size_t i = 0; i < n; ++i) {
                if (p[i] == q[i])
                        continue;

                if (++bad <= shown)
                        printf("%s(%ld, ...): %s byte %zu: rcc %d, clang %d\n",
                               name, x, object, i, p[i], q[i]);
                return 1;
        }

        return 0;
}

static int
pointers_differ(
    const char *name, long x, const char *object, const void *p, const void *q)
{
        const long got = offset(p, base(0));
        const long want = offset(q, base(1));

        if (got == want)
                return 0;

        if (++bad <= shown)
                printf("%s(%ld, ...): %s: rcc B%+ld, clang B%+ld\n", name, x,
                       object, got, want);
        return 1;
}

/* each object of a type, until one differs */
#define OBJECT_DIFFERS(o) \
        differs = differs || bytes_differ(name, x, #o, &o, &ref_##o, sizeof o)
#define POINTER_DIFFERS(o) \
        differs = differs || pointers_differ(name, x, #o, o, ref_##o)
#define CELLS_DIFFER(o)                 \
        for (int j = 0; j < CELLS; ++j) \
        differs = differs || pointers_differ(name, x, #o, o[j], ref_##o[j])
#define TYPE_DIFFERS(T, N)       \
        OBJECT_DIFFERS(d_##N);   \
        OBJECT_DIFFERS(x_##N);   \
        OBJECT_DIFFERS(da_##N);  \
        OBJECT_DIFFERS(xa_##N);  \
        OBJECT_DIFFERS(dm_##N);  \
        OBJECT_DIFFERS(xm_##N);  \
        POINTER_DIFFERS(dp_##N); \
        POINTER_DIFFERS(xp_##N); \
        CELLS_DIFFER(dq_##N);    \
        CELLS_DIFFER(xq_##N);

/* every object, then the buffers */
static void
compare(const char *name, long x)
{
        int differs = 0;

        ++total;

        TYPES(TYPE_DIFFERS)
        if (differs)
                return;

        bytes_differ(name, x, "B", area[0], area[1], sizeof area[0]);
}

/* f and its reference called alike, each on its own objects and its
   own buffer, B, filled alike; the results compared, then the objects */
#define AS_LONG(e)   ((long) (e))
#define AS_VOID(e)   ((e), 0L)
#define AS_OFFSET(e) offset((const void *) (e), B)

#define SIDES(as, f, x, at, arguments)                    \
        do {                                              \
                long got_;                                \
                long want_;                               \
                                                          \
                prime((long) (x), at);                    \
                {                                         \
                        unsigned char *const B = base(0); \
                        got_ = as(f arguments);           \
                }                                         \
                {                                         \
                        unsigned char *const B = base(1); \
                        want_ = as(ref_##f arguments);    \
                }                                         \
                report(#f, (long) (x), got_, want_);      \
                compare(#f, (long) (x));                  \
        } while (0)

#define MCHECK(f, x, at, arguments) SIDES(AS_LONG, f, x, at, arguments)
#define VCHECK(f, x, at, arguments) SIDES(AS_VOID, f, x, at, arguments)
#define PCHECK(f, x, at, arguments) SIDES(AS_OFFSET, f, x, at, arguments)

/* an address into an object, as its offset from its build's object */
#define ACHECK(f, x, at, arguments, object)                                  \
        do {                                                                 \
                long got_;                                                   \
                long want_;                                                  \
                                                                             \
                prime((long) (x), at);                                       \
                got_ = offset((const void *) f arguments, &object);          \
                want_ =                                                      \
                    offset((const void *) ref_##f arguments, &ref_##object); \
                report(#f, (long) (x), got_, want_);                         \
                compare(#f, (long) (x));                                     \
        } while (0)

/* The elements of one kind of object of one type by a variable index,
   and the constants over a row's address, for one value: every index
   into the array. */
#define TEST_ROWS(T, N, K)                                              \
        static void rows_##K##_##N(long x, long y, int at)              \
        {                                                               \
                for (int k = 0; k < ARRAY; ++k) {                       \
                        VCHECK(indexed_##K##_##N, x, at, (y, k));       \
                        MCHECK(indexed_value_##K##_##N, x, at, (y, k)); \
                        MCHECK(indexed_cells_##K##_##N, x, at, (y, k)); \
                        MCHECK(indexed_distance_##K##_##N, x, at, (k)); \
                        VCHECK(summed_##K##_##N, x, at, (y, k));        \
                        MCHECK(summed_value_##K##_##N, x, at, (y, k));  \
                        MCHECK(summed_distance_##K##_##N, x, at, (k));  \
                }                                                       \
        }
#define TEST_ROWS_KINDS(T, N) KINDS(TEST_ROWS, T, N)

TYPES(TEST_ROWS_KINDS)

/* Every function of one kind of object of one type, over every value:
   x is in the scalars and where the pointers point, y an argument, i
   an index into the array, r and c a row and a column, n a shift
   count, s a step of the pointers and q a divisor.  The pointers point
   16 to 40 bytes above B, so a step back of two elements stays in the
   buffer. */
#define TEST_KIND(T, N, K)                                                    \
        static void test_##K##_##N(void)                                      \
        {                                                                     \
                for (int v = 0; v < COUNT; ++v) {                             \
                        const long x = values[v];                             \
                        const long y = values[(v * 7 + 3) % COUNT];           \
                        const int at = 16 + 8 * (v % 4);                      \
                        const int i = (int) (y & (ARRAY - 1));                \
                        const int r = (int) ((unsigned long) y % ROWS);       \
                        const int c = (int) (x & (COLUMNS - 1));              \
                        const int n = (int) (y & 31);                         \
                        const int s = (int) (y & 7) - 4;                      \
                        const int q = divisors[v % DIVISORS];                 \
                                                                              \
                        MCHECK(read_##K##_##N, x, at, ());                    \
                        MCHECK(read_int_##K##_##N, x, at, ());                \
                        VCHECK(write_##K##_##N, x, at, (y));                  \
                        MCHECK(write_value_##K##_##N, x, at, (y));            \
                        VCHECK(zero_##K##_##N, x, at, ());                    \
                        VCHECK(constant_##K##_##N, x, at, ());                \
                        VCHECK(add_##K##_##N, x, at, (y));                    \
                        VCHECK(add_five_##K##_##N, x, at, ());                \
                        MCHECK(add_value_##K##_##N, x, at, (y));              \
                        VCHECK(sub_##K##_##N, x, at, ((int) y));              \
                        VCHECK(xor_##K##_##N, x, at, (y));                    \
                        VCHECK(shl_##K##_##N, x, at, (n));                    \
                        VCHECK(shr_##K##_##N, x, at, (n));                    \
                        MCHECK(shr_value_##K##_##N, x, at, (n));              \
                        VCHECK(mul_##K##_##N, x, at, (y));                    \
                        MCHECK(div_value_##K##_##N, x, at, (q));              \
                        VCHECK(inc_##K##_##N, x, at, ());                     \
                        VCHECK(dec_##K##_##N, x, at, ());                     \
                        MCHECK(postinc_##K##_##N, x, at, ());                 \
                        MCHECK(preinc_##K##_##N, x, at, ());                  \
                        MCHECK(postdec_##K##_##N, x, at, ());                 \
                        MCHECK(again_##K##_##N, x, at, ());                   \
                        MCHECK(around_##K##_##N, x, at, (y));                 \
                                                                              \
                        MCHECK(elem_##K##_##N, x, at, ());                    \
                        MCHECK(elem_at_##K##_##N, x, at, (i));                \
                        VCHECK(put_##K##_##N, x, at, (y));                    \
                        VCHECK(put_at_##K##_##N, x, at, (i, y));              \
                        VCHECK(bump_at_##K##_##N, x, at, (i, y));             \
                        MCHECK(inc_at_##K##_##N, x, at, (i));                 \
                        MCHECK(last_##K##_##N, x, at, ());                    \
                        MCHECK(cell_##K##_##N, x, at, (r, c));                \
                        MCHECK(corner_##K##_##N, x, at, ());                  \
                        MCHECK(origin_##K##_##N, x, at, ());                  \
                        VCHECK(set_cell_##K##_##N, x, at, (r, c, y));         \
                        VCHECK(set_row_one_##K##_##N, x, at, (c, y));         \
                        MCHECK(row_##K##_##N, x, at, (r));                    \
                                                                              \
                        ACHECK(address_##K##_##N, x, at, (), K##_##N);        \
                        ACHECK(array_##K##_##N, x, at, (), K##a_##N);         \
                        ACHECK(elem_address_##K##_##N, x, at, (i), K##a_##N); \
                        ACHECK(row_address_##K##_##N, x, at, (r), K##m_##N);  \
                        MCHECK(pass_##K##_##N, x, at, (y));                   \
                        MCHECK(pass_array_##K##_##N, x, at, (y));             \
                                                                              \
                        PCHECK(pointer_##K##_##N, x, at, ());                 \
                        VCHECK(set_pointer_##K##_##N, x, at,                  \
                               ((T *) (B + 8 * c)));                          \
                        MCHECK(deref_##K##_##N, x, at, ());                   \
                        VCHECK(store_through_##K##_##N, x, at, (y));          \
                        MCHECK(index_##K##_##N, x, at, (i));                  \
                        VCHECK(step_##K##_##N, x, at, ());                    \
                        VCHECK(step_by_##K##_##N, x, at, (s));                \
                        VCHECK(back_##K##_##N, x, at, ());                    \
                        PCHECK(step_value_##K##_##N, x, at, ());              \
                        PCHECK(step_by_value_##K##_##N, x, at, (s));          \
                        MCHECK(walk_##K##_##N, x, at, ());                    \
                        VCHECK(walk_store_##K##_##N, x, at, (y));             \
                                                                              \
                        MCHECK(fold_before_##K##_##N, x, at, ());             \
                        MCHECK(fold_beyond_##K##_##N, x, at, ());             \
                        VCHECK(fold_put_##K##_##N, x, at, (y));               \
                        VCHECK(fold_bump_##K##_##N, x, at, (y));              \
                        VCHECK(fold_shift_##K##_##N, x, at, (n));             \
                        VCHECK(fold_tick_##K##_##N, x, at, ());               \
                        MCHECK(fold_tick_value_##K##_##N, x, at, ());         \
                        MCHECK(fold_bump_value_##K##_##N, x, at, (y));        \
                        MCHECK(fold_sum_##K##_##N, x, at, ());                \
                        MCHECK(fold_minus_##K##_##N, x, at, ());              \
                        MCHECK(fold_nested_##K##_##N, x, at, ());             \
                        ACHECK(fold_third_##K##_##N, x, at, (), K##a_##N);    \
                        ACHECK(fold_plus_##K##_##N, x, at, (), K##a_##N);     \
                        ACHECK(fold_back_##K##_##N, x, at, (), K##m_##N);     \
                        ACHECK(fold_row_##K##_##N, x, at, (), K##m_##N);      \
                        MCHECK(fold_zero_##K##_##N, x, at, (y));              \
                        ACHECK(fold_below_##K##_##N, x, at, (), K##a_##N);    \
                        ACHECK(fold_past_##K##_##N, x, at, (), K##a_##N);     \
                        ACHECK(fold_end_##K##_##N, x, at, (), K##a_##N);      \
                        ACHECK(fold_row_past_##K##_##N, x, at, (), K##m_##N); \
                        MCHECK(fold_distance_##K##_##N, x, at, ());           \
                        rows_##K##_##N(x, y, at);                             \
                                                                              \
                        PCHECK(cell_get_##K##_##N, x, at, ());                \
                        VCHECK(cell_put_##K##_##N, x, at,                     \
                               ((T *) (B + 8 * c)));                          \
                        MCHECK(cell_deref_##K##_##N, x, at, ());              \
                        MCHECK(cell_index_##K##_##N, x, at, ());              \
                        VCHECK(cell_step_##K##_##N, x, at, ());               \
                        VCHECK(cell_store_##K##_##N, x, at, (y));             \
                }                                                             \
        }
#define TEST_KINDS(T, N) KINDS(TEST_KIND, T, N)

TYPES(TEST_KINDS)

/* The initialized objects of both builds, before anything writes them:
   each equals its initializer converted to its type. */
#define CHECK_INITIALIZED(T, name, value)                  \
        report(#name, 0, (long) name, (long) (T) (value)), \
            report("ref_" #name, 0, (long) ref_##name, (long) (T) (value))

static void
test_initialized(void)
{
        INITIALIZED(CHECK_INITIALIZED)

        report("i_null", 0, i_null == 0, 1);
        report("ref_i_null", 0, ref_i_null == 0, 1);
        report("i_void_null", 0, i_void_null == 0, 1);
        report("ref_i_void_null", 0, ref_i_void_null == 0, 1);
        report("i_handler", 0, i_handler == 0, 1);
        report("ref_i_handler", 0, ref_i_handler == 0, 1);

        report("read_twice", 0, read_twice(), 0);
        report("ref_read_twice", 0, ref_read_twice(), 0);
        report("read_extern", 0, read_extern(), 0);
        report("ref_read_extern", 0, ref_read_extern(), 0);
        report("read_late", 0, read_late(), 77);
        report("ref_read_late", 0, ref_read_late(), 77);
}

/* The tentative definitions and the object defined after its use,
   written by one function and read by another and by the main. */
static void
test_tentative(void)
{
        for (int v = 0; v < COUNT; ++v) {
                const long x = values[v];

                write_twice(x);
                ref_write_twice(x);
                report("write_twice", x, read_twice(), ref_read_twice());
                report("t_twice", x, t_twice, ref_t_twice);

                bump_late((int) x);
                ref_bump_late((int) x);
                report("bump_late", x, read_late(), ref_read_late());
                report("late", x, late, ref_late);

                t_extern = ref_t_extern = (int) x;
                report("read_extern", x, read_extern(), ref_read_extern());
        }
}

/* The static array that crosses a page, filled alike in both builds
   through the start each gives: the reads, the writes and the address
   past the boundary, as its offset from the build's own start. */
static void
test_big(void)
{
        char *const own = big_base();
        char *const ref = ref_big_base();

        for (int v = 0; v < COUNT; ++v) {
                const long x = values[v];

                prime(x, 16);
                fill(own, ref, BIG);
                report("big_read", x, big_read(), ref_big_read());

                big_write(x);
                ref_big_write(x);
                report("big_write", x, big_read(), ref_big_read());
                bytes_differ("big_write", x, "big", own, ref, BIG);

                report("big_middle", x, offset(big_middle(), own),
                       offset(ref_big_middle(), ref));
                report("big_end", x, offset(big_end(), own),
                       offset(ref_big_end(), ref));
                report("big_beyond", x, offset(big_beyond(), own),
                       offset(ref_big_beyond(), ref));
                report("big_below", x, offset(big_below(), own),
                       offset(ref_big_below(), ref));
        }
}

int
main(void)
{
        if (getenv("PROBE_ALL"))
                shown = LONG_MAX;

        test_initialized();
        test_tentative();

#define RUN(T, N)     \
        test_d_##N(); \
        test_x_##N();
        TYPES(RUN)

        test_big();

        printf("%ld checks, %ld differ\n", total, bad);

        return bad != 0;
}
