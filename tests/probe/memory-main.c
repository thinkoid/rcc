/* The memory probe's main, compiled by clang -O0: calls every function
   of memory.c as rcc compiled it and as clang did (ref_), over a table
   of values, and compares.  Prints the first forty differences, or
   every one when PROBE_ALL is set in the environment, and a summary;
   exits 0 when nothing differs, 1 otherwise.

   Memory the functions read and write is the main's.  Each build has
   a buffer and eight cells of its own, filled alike before each check:
   B names the middle of the buffer of the build called, C its cells,
   which point into its buffer.  A check compares the results, then
   the buffers' bytes, guard bytes on both sides included, then the
   cells as offsets from B, so a store too wide or at the wrong
   address shows.  A pointer result is compared as its offset from B.
   A pointer that is only compared or converted, never dereferenced,
   is any value of the table, the same for both builds. */

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "memory.h"

static const long values[] = {
    0, 1, 2, 3, 5, 7, 85, 100, 127, 128, 200, 255, 256, 4095, 4096, 4097,
    5000, 32767, 32768, 65535, 65536, 0x12345, 0xfff000, 0x12345678,
    0x7fffffff, 0x80000000L, 0xffffffffL, 0x100000000L, 0x123456789abcdef0L,
    -1, -2, -3, -7, -128, -129, -4096, -5000, -32768, -32769, -65536,
    -2147483647L - 1, -2147483649L, LONG_MAX, LONG_MIN
};

#define COUNT ((int) (sizeof values / sizeof *values))

/* where in the buffer a pointer the main makes points: aligned, and
   misaligned for every width, G11 */
static const int offsets[] = { 0, 1, 3, 4, 7 };

#define OFFSETS ((int) (sizeof offsets / sizeof *offsets))

/* the bytes on each side of B, and the guard beyond them */
#define SPAN 64
#define GUARD 32
#define CELLS 8

static unsigned char area[2][GUARD + 2 * SPAN + GUARD]
    __attribute__((aligned(16)));
static void *cell[2][CELLS];
static unsigned long seed;

static long total;
static long bad;
static long shown = 40;

static unsigned char *
base(int k)
{
    return area[k] + GUARD + SPAN;
}

/* Both buffers filled with the same bytes, which change from check to
   check; x placed at B + at and y 16 bytes after it; the first cell
   pointing at B + at, the others elsewhere in the buffer. */
static void
prime(long x, long y, int at)
{
    ++seed;

    for (int i = 0; i < (int) sizeof area[0]; ++i)
        area[0][i] = area[1][i] =
            (unsigned char) (seed * 0x9e3779b1UL + i * 151UL + (i >> 3));

    for (int k = 0; k < 2; ++k) {
        memcpy(base(k) + at, &x, sizeof x);
        memcpy(base(k) + at + 16, &y, sizeof y);

        cell[k][0] = base(k) + at;
        for (int c = 1; c < CELLS; ++c)
            cell[k][c] = base(k) - SPAN + 12 * c + 1;
    }
}

/* a pointer as its offset from the base of its build's buffer */
static long
offset(const void *p, const unsigned char *b)
{
    if (!p)
        return LONG_MIN;

    return (long) ((unsigned long) p - (unsigned long) b);
}

static void
report(const char *name, long x, long y, long got, long want)
{
    ++total;

    if (got == want)
        return;

    if (++bad <= shown)
        printf("%s(%ld, %ld, ...): rcc %ld, clang %ld\n", name, x, y, got,
               want);
}

/* the buffers byte by byte, and the cells */
static void
compare(const char *name, long x, long y)
{
    ++total;

    for (int i = 0; i < (int) sizeof area[0]; ++i) {
        if (area[0][i] == area[1][i])
            continue;

        if (++bad <= shown)
            printf("%s(%ld, %ld, ...): byte B%+d: rcc %d, clang %d\n", name,
                   x, y, i - GUARD - SPAN, area[0][i], area[1][i]);
        return;
    }

    for (int c = 0; c < CELLS; ++c) {
        const long got = offset(cell[0][c], base(0));
        const long want = offset(cell[1][c], base(1));

        if (got == want)
            continue;

        if (++bad <= shown)
            printf("%s(%ld, %ld, ...): cell %d: rcc B%+ld, clang B%+ld\n",
                   name, x, y, c, got, want);
        return;
    }
}

/* f and its reference on the same arguments, no memory of the main's */
#define CHECK(f, x, y, arguments)                                       \
    report(#f, (long) (x), (long) (y), (long) f arguments,              \
           (long) ref_##f arguments)

/* the result of a call, as a long, as nothing for a void function, or
   as an offset from B */
#define AS_LONG(e) ((long) (e))
#define AS_VOID(e) ((e), 0L)
#define AS_OFFSET(e) offset((const void *) (e), B)

/* f and its reference on the same arguments, each with its own buffer
   and cells, filled alike; the results compared, then the memory */
#define SIDES(as, f, x, y, at, arguments)                               \
    do {                                                                \
        long got_;                                                      \
        long want_;                                                     \
                                                                        \
        prime((long) (x), (long) (y), at);                              \
        {                                                               \
            unsigned char *const B = base(0);                           \
            void **const C = cell[0];                                   \
            got_ = as(f arguments);                                     \
        }                                                               \
        {                                                               \
            unsigned char *const B = base(1);                           \
            void **const C = cell[1];                                   \
            want_ = as(ref_##f arguments);                              \
        }                                                               \
        report(#f, (long) (x), (long) (y), got_, want_);                \
        compare(#f, (long) (x), (long) (y));                            \
    } while (0)

#define MCHECK(f, x, y, at, arguments) SIDES(AS_LONG, f, x, y, at, arguments)
#define VCHECK(f, x, y, at, arguments) SIDES(AS_VOID, f, x, y, at, arguments)
#define PCHECK(f, x, y, at, arguments)                                  \
    SIDES(AS_OFFSET, f, x, y, at, arguments)

/* declarations: each function and its reference */
#define BOTH(R, f, parameters) R f parameters; R ref_##f parameters;

#define DECLARE_TYPE(T, N)                                              \
    BOTH(long, load_##N, (T *)) BOTH(int, load_int_##N, (T *))          \
    BOTH(long, load_add_##N, (T *, long))                               \
    BOTH(int, load_add_int_##N, (T *, int))                             \
    BOTH(long, load_shift_##N, (T *)) BOTH(long, load_div_##N, (T *))   \
    BOTH(int, load_less_##N, (T *, T *)) BOTH(T, load_result_##N, (T *)) \
    BOTH(int, load_test_##N, (T *)) BOTH(int, load_not_##N, (T *))      \
    BOTH(void, store_##N, (T *, long))                                  \
    BOTH(long, store_value_##N, (T *, long))                            \
    BOTH(long, store_int_##N, (T *, int))                               \
    BOTH(long, store_narrow_##N, (T *, signed char))                    \
    BOTH(long, copy_##N, (T *, T *)) BOTH(long, chain_##N, (T *, T *, long)) \
    BOTH(long, load2_##N, (T **)) BOTH(long, store2_##N, (T **, long))  \
    BOTH(T *, fetch_##N, (T **)) BOTH(void, retarget_##N, (T **, T *))  \
    BOTH(long, through_##N, (T **, T *, long))                          \
    BOTH(long, wide_add_##N, (T *, long))                               \
    BOTH(void, ewide_xor_##N, (T *, long))                              \
    BOTH(long, preinc_##N, (T *)) BOTH(long, postinc_##N, (T *))        \
    BOTH(long, predec_##N, (T *)) BOTH(long, postdec_##N, (T *))        \
    BOTH(void, steps_##N, (T *, T *)) BOTH(long, stepped_##N, (T *))    \
    BOTH(long, addr_local_##N, (T)) BOTH(long, addr_param_##N, (T))     \
    BOTH(long, addr_held_##N, (T)) BOTH(long, star_amp_##N, (T))        \
    BOTH(T *, amp_star_##N, (T *))                                      \
    BOTH(long, amp_star_store_##N, (T *, long))                         \
    BOTH(long, addr_pp_##N, (T)) BOTH(long, addr_two_##N, (T, T))       \
    BOTH(long, array_##N, (T)) BOTH(long, array_fill_##N, (int, long))  \
    BOTH(long, matrix_##N, (long))                                      \
    BOTH(long, ninth_##N, (P8(long), T *, T *))                         \
    BOTH(T *, pick_##N, (T *, T *, int))                                \
    BOTH(T *, pick9_##N, (P8(long), T *))                               \
    BOTH(int, eq_##N, (T *, T *)) BOTH(int, ne_##N, (T *, T *))         \
    BOTH(int, lt_##N, (T *, T *)) BOTH(int, le_##N, (T *, T *))         \
    BOTH(int, gt_##N, (T *, T *)) BOTH(int, ge_##N, (T *, T *))         \
    BOTH(int, iflt_##N, (T *, T *)) BOTH(int, ifge_##N, (T *, T *))     \
    BOTH(int, ifeq_##N, (T *, T *))                                     \
    BOTH(int, null_##N, (T *)) BOTH(int, nonnull_##N, (T *))            \
    BOTH(int, truth_##N, (T *)) BOTH(int, not_##N, (T *))               \
    BOTH(int, and_##N, (T *, T *)) BOTH(int, or_##N, (T *, T *))        \
    BOTH(int, both_##N, (T *, T *)) BOTH(long, sel_##N, (T *, long, long)) \
    BOTH(T *, either_##N, (T *, T *))                                   \
    BOTH(long, deref_pick_##N, (T *, T *))                              \
    BOTH(long, to_long_##N, (T *))                                      \
    BOTH(unsigned long, to_ulong_##N, (T *))                            \
    BOTH(int, to_int_##N, (T *)) BOTH(T *, from_long_##N, (long))       \
    BOTH(T *, from_ulong_##N, (unsigned long))                          \
    BOTH(T *, from_int_##N, (int)) BOTH(T *, from_unsigned_##N, (unsigned)) \
    BOTH(T *, from_char_##N, (signed char)) BOTH(T *, round_##N, (T *)) \
    BOTH(T *, voided_##N, (T *)) BOTH(long, view_load_##N, (char *))    \
    BOTH(void, view_store_##N, (int *, long))                           \
    BOTH(long, view_back_##N, (T *))                                    \
    BOTH(long, far_##N, (T)) BOTH(long, farther_##N, (T))               \
    BOTH(long, far_held_##N, (T *, T))                                  \
    BOTH(long, farther_held_##N, (T *, T))

TYPES(DECLARE_TYPE)

#define DECLARE_COMPOUND(T, N, name, op)                                \
    BOTH(long, name##_eq_##N, (T *, T))                                 \
    BOTH(void, name##_on_##N, (T *, T))
#define DECLARE_COMPOUND_ALL(T, N) COMPOUNDS(DECLARE_COMPOUND, T, N)

TYPES(DECLARE_COMPOUND_ALL)

#define DECLARE_SHIFT(T, N, name, op)                                   \
    BOTH(long, name##_eq_##N, (T *, int))                               \
    BOTH(void, name##_on_##N, (T *, int))
#define DECLARE_SHIFT_ALL(T, N) SHIFTS(DECLARE_SHIFT, T, N)

TYPES(DECLARE_SHIFT_ALL)

#define DECLARE_KCOMPOUND(T, N, name, op, constant)                     \
    BOTH(long, k##name##_##N, (T *)) BOTH(void, ek##name##_##N, (T *))
#define DECLARE_KCOMPOUND_ALL(T, N) KCOMPOUNDS(DECLARE_KCOMPOUND, T, N)

TYPES(DECLARE_KCOMPOUND_ALL)

#define DECLARE_STORE(T, N, name, constant)                             \
    BOTH(void, k##name##_store_##N, (T *))                              \
    BOTH(long, k##name##_value_##N, (T *))
#define DECLARE_STORE_ALL(T, N) STORES(DECLARE_STORE, T, N)

TYPES(DECLARE_STORE_ALL)

BOTH(long, text_pass, (void))
BOTH(long, text_two, (void))
BOTH(long, text_escapes, (void))
BOTH(long, text_empty, (void))
BOTH(int, text_first, (void))
BOTH(int, text_star, (void))
BOTH(long, text_held, (long))
BOTH(int, text_cond, (int))
BOTH(long, text_arg, (int))
BOTH(long, text_joined, (void))
BOTH(long, far_array, (int, long))
BOTH(long, far_matrix, (long))
BOTH(long, far_param, (long, int))
BOTH(long, deep_load, (P8(long), long *))
BOTH(long, deep_store, (P8(long), int *))
BOTH(long, deep_twice, (P8(long), short **))
BOTH(long, around, (long *, long *, long))
BOTH(long, around_held, (int *, long))
BOTH(long, around_store, (char *, long))

/* The loads and stores, the pointers to pointers and the steps of one
   type, over every value and every offset: P(T) is a pointer the main
   made at B + at, Q(T) one 16 bytes after it, holding y. */
#define TEST_ACCESS(T, N)                                               \
    static void test_access_##N(void)                                   \
    {                                                                   \
        for (int i = 0; i < COUNT; ++i) {                               \
            for (int o = 0; o < OFFSETS; ++o) {                         \
                const long x = values[i];                               \
                const long y = values[(i * 7 + o + 3) % COUNT];         \
                const int at = offsets[o];                              \
                                                                        \
                MCHECK(load_##N, x, at, at, (P(T)));                    \
                MCHECK(load_int_##N, x, at, at, (P(T)));                \
                MCHECK(load_add_##N, x, y, at, (P(T), y));              \
                MCHECK(load_add_int_##N, x, y, at, (P(T), (int) y));    \
                MCHECK(load_shift_##N, x, at, at, (P(T)));              \
                MCHECK(load_div_##N, x, at, at, (P(T)));                \
                MCHECK(load_less_##N, x, y, at, (P(T), Q(T)));          \
                MCHECK(load_result_##N, x, at, at, (P(T)));             \
                MCHECK(load_test_##N, x, at, at, (P(T)));               \
                MCHECK(load_not_##N, x, at, at, (P(T)));                \
                VCHECK(store_##N, x, y, at, (P(T), y));                 \
                MCHECK(store_value_##N, x, y, at, (P(T), y));           \
                MCHECK(store_int_##N, x, y, at, (P(T), (int) y));       \
                MCHECK(store_narrow_##N, x, y, at,                      \
                       (P(T), (signed char) y));                        \
                MCHECK(copy_##N, x, y, at, (P(T), Q(T)));               \
                MCHECK(chain_##N, x, y, at, (P(T), Q(T), x ^ y));       \
                                                                        \
                MCHECK(load2_##N, x, at, at, ((T **) C));               \
                MCHECK(store2_##N, x, y, at, ((T **) C, y));            \
                PCHECK(fetch_##N, x, at, at, ((T **) C));               \
                VCHECK(retarget_##N, x, at, at, ((T **) C + 2, Q(T)));  \
                MCHECK(through_##N, x, y, at, ((T **) C + 1, Q(T), x)); \
                                                                        \
                MCHECK(preinc_##N, x, at, at, (P(T)));                  \
                MCHECK(postinc_##N, x, at, at, (P(T)));                 \
                MCHECK(predec_##N, x, at, at, (P(T)));                  \
                MCHECK(postdec_##N, x, at, at, (P(T)));                 \
                VCHECK(steps_##N, x, y, at, (P(T), Q(T)));              \
                MCHECK(stepped_##N, x, at, at, (P(T)));                 \
                                                                        \
                MCHECK(view_load_##N, x, at, at, ((char *) (B + at)));  \
                VCHECK(view_store_##N, x, y, at, ((int *) (B + at), y)); \
                MCHECK(view_back_##N, x, at, at, (P(T)));               \
            }                                                           \
        }                                                               \
    }

#define P(T) ((T *) (B + at))
#define Q(T) ((T *) (B + at + 16))

TYPES(TEST_ACCESS)

/* every compound assignment through P(T), its value and its effect, on
   an object x and an operand y; a divisor of 0 or -1 skipped, as in
   the straight probe; a count below the promoted width */
#define TEST_COMPOUND(T, N, name, op)                                   \
    MCHECK(name##_eq_##N, x, y, at, (P(T), (T) y));                     \
    VCHECK(name##_on_##N, x, y, at, (P(T), (T) y));

#define TEST_KCOMPOUND(T, N, name, op, constant)                        \
    MCHECK(k##name##_##N, x, 0, at, (P(T)));                            \
    VCHECK(ek##name##_##N, x, 0, at, (P(T)));

#define TEST_OPERATORS(T, N)                                            \
    static void test_operators_##N(void)                                \
    {                                                                   \
        const int width = (int) (sizeof(+(T) 0) * CHAR_BIT);            \
                                                                        \
        for (int i = 0; i < COUNT; ++i) {                               \
            const long x = values[i];                                   \
            const int at = i % 2 ? 3 : 0;                               \
                                                                        \
            KCOMPOUNDS(TEST_KCOMPOUND, T, N)                            \
                                                                        \
            for (int n = 0; n < width; n += 3) {                        \
                MCHECK(shl_eq_##N, x, n, at, (P(T), n));                \
                VCHECK(shl_on_##N, x, n, at, (P(T), n));                \
                MCHECK(shr_eq_##N, x, n, at, (P(T), n));                \
                VCHECK(shr_on_##N, x, n, at, (P(T), n));                \
            }                                                           \
                                                                        \
            for (int j = i % 7; j < COUNT; j += 7) {                    \
                const long y = values[j];                               \
                                                                        \
                TEST_COMPOUND(T, N, add, +=)                            \
                TEST_COMPOUND(T, N, sub, -=)                            \
                TEST_COMPOUND(T, N, mul, *=)                            \
                TEST_COMPOUND(T, N, and, &=)                            \
                TEST_COMPOUND(T, N, or, |=)                             \
                TEST_COMPOUND(T, N, xor, ^=)                            \
                MCHECK(wide_add_##N, x, y, at, (P(T), y));              \
                VCHECK(ewide_xor_##N, x, y, at, (P(T), y));             \
                                                                        \
                if ((T) y != 0 && (T) y != (T) -1) {                    \
                    TEST_COMPOUND(T, N, div, /=)                        \
                    TEST_COMPOUND(T, N, rem, %=)                        \
                }                                                       \
            }                                                           \
        }                                                               \
                                                                        \
        for (int o = 0; o < OFFSETS; ++o) {                             \
            const int at = offsets[o];                                  \
                                                                        \
            for (int i = 0; i < COUNT; i += 11) {                       \
                const long x = values[i];                               \
                                                                        \
                STORES(TEST_STORE, T, N)                                \
            }                                                           \
        }                                                               \
    }

#define TEST_STORE(T, N, name, constant)                                \
    VCHECK(k##name##_store_##N, x, at, at, (P(T)));                     \
    MCHECK(k##name##_value_##N, x, at, at, (P(T)));

TYPES(TEST_OPERATORS)

/* the addresses of locals and parameters, the arrays, the frames, and
   pointers as values of one type */
#define TEST_ADDRESS(T, N)                                              \
    static void test_address_##N(void)                                  \
    {                                                                   \
        for (int i = 0; i < COUNT; ++i) {                               \
            const long x = values[i];                                   \
            const long y = values[(i * 5 + 1) % COUNT];                 \
            const int at = i % 3;                                       \
                                                                        \
            CHECK(addr_local_##N, x, 0, ((T) x));                       \
            CHECK(addr_param_##N, x, 0, ((T) x));                       \
            CHECK(addr_held_##N, x, 0, ((T) x));                        \
            CHECK(star_amp_##N, x, 0, ((T) x));                         \
            CHECK(addr_pp_##N, x, 0, ((T) x));                          \
            CHECK(addr_two_##N, x, y, ((T) x, (T) y));                  \
            PCHECK(amp_star_##N, x, at, at, (P(T)));                    \
            MCHECK(amp_star_store_##N, x, y, at, (P(T), y));            \
                                                                        \
            CHECK(array_##N, x, 0, ((T) x));                            \
            CHECK(array_fill_##N, x, i % 16 + 1, (i % 16 + 1, x));      \
            CHECK(matrix_##N, x, 0, (x));                               \
                                                                        \
            CHECK(far_##N, x, 0, ((T) x));                              \
            CHECK(farther_##N, x, 0, ((T) x));                          \
            MCHECK(far_held_##N, x, y, at, (P(T), (T) y));              \
            MCHECK(farther_held_##N, x, y, at, (P(T), (T) y));          \
                                                                        \
            MCHECK(ninth_##N, x, y, at,                                 \
                   (x, y, 1, 2, 3, 4, 5, y - x, P(T), Q(T)));           \
            PCHECK(pick_##N, x, 1, at, (P(T), Q(T), 1));                \
            PCHECK(pick_##N, x, 0, at, (P(T), Q(T), 0));                \
            PCHECK(pick9_##N, x, y, at, (x, y, x, y, x, y, x, y, Q(T))); \
            MCHECK(deref_pick_##N, x, y, at, (P(T), Q(T)));             \
                                                                        \
            CHECK(to_long_##N, x, 0, ((T *) x));                        \
            CHECK(to_ulong_##N, x, 0, ((T *) x));                       \
            CHECK(to_int_##N, x, 0, ((T *) x));                         \
            CHECK(from_long_##N, x, 0, (x));                            \
            CHECK(from_ulong_##N, x, 0, ((unsigned long) x));           \
            CHECK(from_int_##N, x, 0, ((int) x));                       \
            CHECK(from_unsigned_##N, x, 0, ((unsigned) x));             \
            CHECK(from_char_##N, x, 0, ((signed char) x));              \
            CHECK(round_##N, x, 0, ((T *) x));                          \
            CHECK(voided_##N, x, 0, ((T *) x));                         \
            CHECK(null_##N, x, 0, ((T *) x));                           \
            CHECK(nonnull_##N, x, 0, ((T *) x));                        \
            CHECK(truth_##N, x, 0, ((T *) x));                          \
            CHECK(not_##N, x, 0, ((T *) x));                            \
            CHECK(sel_##N, x, y, ((T *) x, x, y));                      \
                                                                        \
            for (int j = i % 5; j < COUNT; j += 5) {                    \
                const long z = values[j];                               \
                                                                        \
                CHECK(eq_##N, x, z, ((T *) x, (T *) z));                \
                CHECK(ne_##N, x, z, ((T *) x, (T *) z));                \
                CHECK(lt_##N, x, z, ((T *) x, (T *) z));                \
                CHECK(le_##N, x, z, ((T *) x, (T *) z));                \
                CHECK(gt_##N, x, z, ((T *) x, (T *) z));                \
                CHECK(ge_##N, x, z, ((T *) x, (T *) z));                \
                CHECK(iflt_##N, x, z, ((T *) x, (T *) z));              \
                CHECK(ifge_##N, x, z, ((T *) x, (T *) z));              \
                CHECK(ifeq_##N, x, z, ((T *) x, (T *) z));              \
                CHECK(and_##N, x, z, ((T *) x, (T *) z));               \
                CHECK(or_##N, x, z, ((T *) x, (T *) z));                \
                CHECK(both_##N, x, z, ((T *) x, (T *) z));              \
                CHECK(either_##N, x, z, ((T *) x, (T *) z));            \
            }                                                           \
                                                                        \
            CHECK(eq_##N, x, x, ((T *) x, (T *) x));                    \
            CHECK(le_##N, x, x, ((T *) x, (T *) x));                    \
            CHECK(gt_##N, x, x, ((T *) x, (T *) x));                    \
        }                                                               \
    }

TYPES(TEST_ADDRESS)

/* the literals, the frames of one function each, and depth */
static void
test_rest(void)
{
    CHECK(text_pass, 0, 0, ());
    CHECK(text_two, 0, 0, ());
    CHECK(text_escapes, 0, 0, ());
    CHECK(text_empty, 0, 0, ());
    CHECK(text_first, 0, 0, ());
    CHECK(text_star, 0, 0, ());
    CHECK(text_joined, 0, 0, ());

    for (int c = -1; c < 3; ++c) {
        CHECK(text_cond, c, 0, (c));
        CHECK(text_arg, c, 0, (c));
    }

    for (int i = 0; i < COUNT; ++i) {
        for (int k = 1; k < COUNT; k += 3) {
            long v[8];

            for (int p = 0; p < 8; ++p)
                v[p] = values[(i + p * k) % COUNT];

            const long x = v[0];
            const long y = v[1];
            const int at = (i + k) % 8;

            CHECK(text_held, x, 0, (x));
            CHECK(far_array, x, k % 16 + 1, (k % 16 + 1, x));
            CHECK(far_matrix, x, 0, (x));
            CHECK(far_param, x, y, (x, (int) y));

            MCHECK(deep_load, x, y, at,
                   (v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7],
                    (long *) (B + at)));
            MCHECK(deep_store, x, y, at,
                   (v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7],
                    (int *) (B + at)));
            MCHECK(deep_twice, x, y, at,
                   (v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7],
                    (short **) C));
            MCHECK(around, x, y, at,
                   ((long *) (B + at), (long *) (B + at + 16), v[2]));
            MCHECK(around_held, x, y, at, ((int *) (B + at), v[2]));
            MCHECK(around_store, x, y, at, ((char *) (B + at), v[2]));
        }
    }
}

int
main(void)
{
    if (getenv("PROBE_ALL"))
        shown = LONG_MAX;

#define RUN(T, N) test_access_##N(); test_operators_##N(); test_address_##N();
    TYPES(RUN)

    test_rest();

    printf("%ld checks, %ld differ\n", total, bad);

    return bad != 0;
}
