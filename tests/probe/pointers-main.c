/* The pointers probe's main, compiled by clang -O0: calls every
   function of pointers.c as rcc compiled it and as clang did (ref_),
   over a table of values, and compares.  Prints the first forty
   differences, or every one when PROBE_ALL is set in the environment,
   and a summary; exits 0 when nothing differs, 1 otherwise.

   Memory the functions read and write is the main's, as in the memory
   probe: each build has a buffer and eight cells of its own, filled
   alike before each check; B names the middle of the buffer of the
   build called, C its cells.  A check compares the results, then the
   bytes of the span of the buffer in use, guard bytes on both sides
   included, then the cells as offsets from B.  A pointer result is
   compared as its offset from B.  A pointer that is only computed and
   compared, never dereferenced, is any value of the table, the same
   for both builds, and so is its result.

   p - q is compared with clang's only where the language's division
   and clang's agree: clang divides by a power of two with an
   arithmetic shift, since the standard lets it assume the difference
   is a multiple of the size.  For a negative difference that is not,
   rcc's result is compared with the quotient this main computes, the
   byte difference divided by the size and truncated (G6: a
   division), and the line printed says "clang" of that quotient. */

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pointers.h"

static const long values[] = {
    0, 1, 2, 3, 5, 7, 85, 100, 127, 128, 200, 255, 256, 4095, 4096, 4097,
    5000, 32767, 32768, 65535, 65536, 0x12345, 0xfff000, 0x12345678,
    0x7fffffff, 0x80000000L, 0xffffffffL, 0x100000000L, 0x123456789abcdef0L,
    -1, -2, -3, -7, -128, -129, -4096, -5000, -32768, -32769, -65536,
    -2147483647L - 1, -2147483649L, LONG_MAX, LONG_MIN
};

#define COUNT ((int) (sizeof values / sizeof *values))

/* the indexes of a subscript into the buffer, each kept when its type
   holds it, so that it reaches no further than REACH elements */
static const long indexes[] = {
    0, 1, 2, 3, 7, 15, 31, 63, -1, -2, -8, -63
};

#define INDEX_COUNT ((int) (sizeof indexes / sizeof *indexes))
#define REACH 63

/* the bytes on each side of B the widest check uses, a constant
   subscript of 4096 longs and a little more; the span of the
   ordinary check; the guard beyond either */
#define WIDEST (4097 * 8 + 64)
#define SPAN 640
#define GUARD 32
#define CELLS 8

static unsigned char area[2][GUARD + 2 * WIDEST + GUARD]
    __attribute__((aligned(16)));
static void *cell[2][CELLS];
static unsigned long seed;
static int span = SPAN;

static long total;
static long bad;
static long shown = 40;

static unsigned char *
base(int k)
{
    return area[k] + GUARD + WIDEST;
}

/* Both buffers' spans filled with the same bytes, which change from
   check to check; x placed at B + at and y 16 bytes after it; the
   first cell pointing at B + at, the others elsewhere in the span. */
static void
prime(long x, long y, int at)
{
    ++seed;

    for (int i = -span - GUARD; i < span + GUARD; ++i)
        base(0)[i] = base(1)[i] =
            (unsigned char) (seed * 0x9e3779b1UL + i * 151UL + (i >> 3));

    for (int k = 0; k < 2; ++k) {
        memcpy(base(k) + at, &x, sizeof x);
        memcpy(base(k) + at + 16, &y, sizeof y);

        cell[k][0] = base(k) + at;
        for (int c = 1; c < CELLS; ++c)
            cell[k][c] = base(k) - 200 + 48 * c + c % 3;
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

/* the spans byte by byte, and the cells */
static void
compare(const char *name, long x, long y)
{
    ++total;

    for (int i = -span - GUARD; i < span + GUARD; ++i) {
        if (base(0)[i] == base(1)[i])
            continue;

        if (++bad <= shown)
            printf("%s(%ld, %ld, ...): byte B%+d: rcc %d, clang %d\n", name,
                   x, y, i, base(0)[i], base(1)[i]);
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

/* v with its bits above the given bytes made stale, so that a type of
   that many bytes reads v and any wider one does not */
static long
stale(long v, int bytes)
{
    if (bytes >= 8)
        return v;

    const unsigned long mask = (1UL << (8 * bytes)) - 1;

    return (long) (((unsigned long) v & mask) |
                   (0xa5a5a5a5a5a5a5a5UL & ~mask));
}

/* whether p - q of the given size is compared: the byte difference d
   a multiple of the size, or not negative, or the size no power of
   two */
static int
divides(long d, long size)
{
    if (d % size == 0 || d > 0)
        return 1;

    return (size & (size - 1)) != 0;
}

/* p - q where clang's shift and the division part: rcc's result
   against the truncated quotient of the byte difference d */
#define TRUNCATED(f, p, q, d, size)                                     \
    report(#f, (long) (p), (long) (q),                                  \
           (long) f((void *) (p), (void *) (q)), (long) ((d) / (size)))

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

/* a pointer the main made at B + at, of any pointee */
#define P ((void *) (B + at))

/* declarations: each function and its reference */
#define BOTH(R, f, parameters) R f parameters; R ref_##f parameters;

#define DECLARE_ARITH(E, T, S, I, N)                                    \
    BOTH(char *, add_##E##_##N, (T p S, I))                             \
    BOTH(char *, radd_##E##_##N, (T p S, I))                            \
    BOTH(char *, sub_##E##_##N, (T p S, I))                             \
    BOTH(char *, cadd_##E##_##N, (T p S, long))                         \
    BOTH(char *, csub_##E##_##N, (T p S, long))
#define DECLARE_ARITH_ALL(E, T, S) INDEXES(DECLARE_ARITH, E, T, S)

POINTEES(DECLARE_ARITH_ALL)

#define DECLARE_KARITH(E, T, S, name, n)                                \
    BOTH(char *, kadd_##E##_##name, (T p S))                            \
    BOTH(char *, kradd_##E##_##name, (T p S))                           \
    BOTH(char *, ksub_##E##_##name, (T p S))                            \
    BOTH(char *, kaddeq_##E##_##name, (T p S))                          \
    BOTH(char *, ksubeq_##E##_##name, (T p S))
#define DECLARE_KARITH_ALL(E, T, S) KONSTS(DECLARE_KARITH, E, T, S)

POINTEES(DECLARE_KARITH_ALL)

#define DECLARE_POINTER(E, T, S)                                        \
    BOTH(long, diff_##E, (T p S, T q S))                                \
    BOTH(int, diff_less_##E, (T p S, T q S))                            \
    BOTH(long, diff_held_##E, (T p S, T q S, long))                     \
    BOTH(char *, preinc_##E, (T p S)) BOTH(char *, postinc_##E, (T p S)) \
    BOTH(char *, predec_##E, (T p S)) BOTH(char *, postdec_##E, (T p S)) \
    BOTH(long, post_both_##E, (T p S)) BOTH(char *, steps_##E, (T p S)) \
    BOTH(char *, addeq_##E, (T p S, long))                              \
    BOTH(char *, subeq_##E, (T p S, long))                              \
    BOTH(char *, addeq_int_##E, (T p S, int))                           \
    BOTH(char *, subeq_unsigned_##E, (T p S, unsigned))                 \
    BOTH(char *, addeq_char_##E, (T p S, signed char))                  \
    BOTH(int, lt_derived_##E, (T p S, long, long))                      \
    BOTH(int, ge_derived_##E, (T p S, long, T q S))                     \
    BOTH(int, eq_derived_##E, (T p S, long, T q S))                     \
    BOTH(int, le_stepped_##E, (T p S, T q S))                           \
    BOTH(long, wrap_##E, (T p S, long))                                 \
    static long size_##E(T p S) { return sizeof *p; }

POINTEES(DECLARE_POINTER)

#define DECLARE_SUBSCRIPT(E, T, S, I, N)                                \
    BOTH(long, get_##E##_##N, (T *, I))                                 \
    BOTH(long, rget_##E##_##N, (T *, I))                                \
    BOTH(void, put_##E##_##N, (T *, I, long))                           \
    BOTH(long, putv_##E##_##N, (T *, I, long))                          \
    BOTH(long, addto_##E##_##N, (T *, I, T))                            \
    BOTH(void, addon_##E##_##N, (T *, I, T))                            \
    BOTH(void, mulon_##E##_##N, (T *, I, T))                            \
    BOTH(long, inc_##E##_##N, (T *, I))                                 \
    BOTH(long, postdec_##E##_##N, (T *, I))                             \
    BOTH(void, steps_##E##_##N, (T *, I))                               \
    BOTH(char *, addr_##E##_##N, (T *, I))                              \
    BOTH(char *, addr_star_##E##_##N, (T *, I))                         \
    BOTH(long, cget_##E##_##N, (T *, long))                             \
    BOTH(void, cput_##E##_##N, (T *, long, long))                       \
    BOTH(char *, caddr_##E##_##N, (T *, long))                          \
    BOTH(long, aget_##E##_##N, (I, long))                               \
    BOTH(long, aaddr_##E##_##N, (I))
#define DECLARE_SUBSCRIPT_ALL(E, T, S) INDEXES(DECLARE_SUBSCRIPT, E, T, S)

ELEMENTS(DECLARE_SUBSCRIPT_ALL)

#define DECLARE_KSUB(E, T, S, name, c)                                  \
    BOTH(long, kget_##E##_##name, (T *))                                \
    BOTH(void, kput_##E##_##name, (T *, long))                          \
    BOTH(long, kinc_##E##_##name, (T *))
#define DECLARE_KSUB_ALL(E, T, S) KSUBS(DECLARE_KSUB, E, T, S)

ELEMENTS(DECLARE_KSUB_ALL)

#define DECLARE_KBYTE(E, T, S, name, d)                                 \
    BOTH(long, kview_##E##_##name, (char *))                            \
    BOTH(void, kview_put_##E##_##name, (char *, long))                  \
    BOTH(long, kview_index_##E##_##name, (char *))
#define DECLARE_KBYTE_ALL(E, T, S) KBYTES(DECLARE_KBYTE, E, T, S)

ELEMENTS(DECLARE_KBYTE_ALL)

#define DECLARE_WALKS(E, T, S)                                          \
    BOTH(void, cell_steps_##E, (T **))                                  \
    BOTH(char *, cell_post_##E, (T **))                                 \
    BOTH(char *, cell_pre_##E, (T **))                                  \
    BOTH(char *, cell_addeq_##E, (T **, long))                          \
    BOTH(long, walk_##E, (T *, long)) BOTH(long, walk_back_##E, (T *))  \
    BOTH(long, sum_##E, (T *, T *)) BOTH(long, sum_back_##E, (T *, T *)) \
    BOTH(long, fill_##E, (T *, T *, long))                              \
    BOTH(long, pp_##E, (T **, long, long))                              \
    BOTH(void, pp_put_##E, (T **, int, int, long))                      \
    BOTH(long, mat_##E, (int, int, long))                               \
    BOTH(long, pmat_##E, (T m[][5], long, long))                        \
    BOTH(void, pmat_put_##E, (T m[][5], int, unsigned, long))           \
    BOTH(char *, prow_##E, (T m[][5], long))                            \
    BOTH(long, loop_index_##E, (int, long))                             \
    BOTH(long, loop_walk_##E, (int, long))                              \
    BOTH(long, ends_##E, (int, long)) BOTH(long, vast_##E, (long, long))

SCALARS(DECLARE_WALKS)

BOTH(int, lit_index, (int))
BOTH(int, lit_rindex, (long))
BOTH(long, lit_walk, (void))
BOTH(long, lit_length, (void))
BOTH(long, deep_index, (P8(long), long *, int))
BOTH(long, deep_put, (P8(long), int *, long))
BOTH(long, deep_addr, (P8(long), short *, int))
BOTH(long, deep_diff, (P8(long), int *, int *))
BOTH(long, call_index, (long *, long))
BOTH(long, call_put, (int *, long, long))
BOTH(long, call_local, (int, long))
BOTH(char *, call_add, (short *, long))

/* p + n, n + p and p - n of one pointee and one index type: on a
   pointer of the table, the result compared whole, so the wrap of G6
   shows; and on B, the result compared as its offset */
#define TEST_ARITH(E, T, S, I, N)                                       \
    for (int i = 0; i < COUNT; ++i) {                                   \
        const long p = values[(i * 7 + 5) % COUNT];                     \
        const long n = values[i];                                       \
        const long x = stale(n, (int) sizeof(I));                       \
        const int at = i % 4;                                           \
                                                                        \
        CHECK(add_##E##_##N, p, n, ((void *) p, (I) n));                \
        CHECK(radd_##E##_##N, p, n, ((void *) p, (I) n));               \
        CHECK(sub_##E##_##N, p, n, ((void *) p, (I) n));                \
        CHECK(cadd_##E##_##N, p, x, ((void *) p, x));                   \
        CHECK(csub_##E##_##N, p, x, ((void *) p, x));                   \
        PCHECK(add_##E##_##N, at, n, at, (P, (I) n));                   \
        PCHECK(sub_##E##_##N, at, n, at, (P, (I) n));                   \
        PCHECK(cadd_##E##_##N, at, x, at, (P, x));                      \
    }
#define TEST_ARITH_ALL(E, T, S)                                         \
    static void test_arith_##E(void) { INDEXES(TEST_ARITH, E, T, S) }

POINTEES(TEST_ARITH_ALL)

#define TEST_KARITH(E, T, S, name, n)                                   \
    CHECK(kadd_##E##_##name, p, 0, ((void *) p));                       \
    CHECK(kradd_##E##_##name, p, 0, ((void *) p));                      \
    CHECK(ksub_##E##_##name, p, 0, ((void *) p));                       \
    CHECK(kaddeq_##E##_##name, p, 0, ((void *) p));                     \
    CHECK(ksubeq_##E##_##name, p, 0, ((void *) p));                     \
    PCHECK(kadd_##E##_##name, at, 0, at, (P));

/* p - q, the steps, p += n and p -= n, and the comparisons of derived
   pointers, of one pointee, on pointers of the table */
#define TEST_POINTER(E, T, S)                                           \
    static void test_pointer_##E(void)                                  \
    {                                                                   \
        const long size = size_##E(0);                                  \
                                                                        \
        for (int i = 0; i < COUNT; ++i) {                               \
            const long p = values[i];                                   \
            const long n = values[(i * 5 + 3) % COUNT];                 \
            const int at = i % 8;                                       \
                                                                        \
            KONSTS(TEST_KARITH, E, T, S)                                \
                                                                        \
            CHECK(preinc_##E, p, 0, ((void *) p));                      \
            CHECK(postinc_##E, p, 0, ((void *) p));                     \
            CHECK(predec_##E, p, 0, ((void *) p));                      \
            CHECK(postdec_##E, p, 0, ((void *) p));                     \
            CHECK(post_both_##E, p, 0, ((void *) p));                   \
            CHECK(steps_##E, p, 0, ((void *) p));                       \
            CHECK(addeq_##E, p, n, ((void *) p, n));                    \
            CHECK(subeq_##E, p, n, ((void *) p, n));                    \
            CHECK(addeq_int_##E, p, n, ((void *) p, (int) n));          \
            CHECK(subeq_unsigned_##E, p, n, ((void *) p, (unsigned) n)); \
            CHECK(addeq_char_##E, p, n, ((void *) p, (signed char) n)); \
            CHECK(wrap_##E, p, n, ((void *) p, n));                     \
            PCHECK(postinc_##E, at, 0, at, (P));                        \
            PCHECK(subeq_##E, at, n, at, (P, n));                       \
                                                                        \
            for (int j = i % 3; j < COUNT; j += 3) {                    \
                const long q = values[j];                               \
                const long m = values[(j * 11 + 1) % COUNT];            \
                const long d = (long) ((unsigned long) p - q);          \
                const long held = (long) ((unsigned long) d + n * size); \
                                                                        \
                if (divides(d, size)) {                                 \
                    CHECK(diff_##E, p, q, ((void *) p, (void *) q));    \
                    CHECK(diff_less_##E, p, q, ((void *) p, (void *) q)); \
                } else                                                  \
                    TRUNCATED(diff_##E, p, q, d, size);                 \
                                                                        \
                if (divides(held, size))                                \
                    CHECK(diff_held_##E, p, q,                          \
                          ((void *) p, (void *) q, n));                 \
                                                                        \
                CHECK(lt_derived_##E, p, q, ((void *) p, q, m));        \
                CHECK(ge_derived_##E, p, q, ((void *) p, m, (void *) q)); \
                CHECK(eq_derived_##E, p, q, ((void *) p, m, (void *) q)); \
                CHECK(le_stepped_##E, p, q, ((void *) p, (void *) q));  \
            }                                                           \
                                                                        \
            for (int k = -5; k < 6; ++k) {                              \
                const long q = (long) ((unsigned long) p + k);          \
                                                                        \
                if (divides((long) ((unsigned long) p - q), size))      \
                    CHECK(diff_##E, p, q, ((void *) p, (void *) q));    \
                else                                                    \
                    TRUNCATED(diff_##E, p, q,                           \
                              (long) ((unsigned long) p - q), size);    \
                if (divides((long) ((unsigned long) q - p), size))      \
                    CHECK(diff_##E, q, p, ((void *) q, (void *) p));    \
                else                                                    \
                    TRUNCATED(diff_##E, q, p,                           \
                              (long) ((unsigned long) q - p), size);    \
                CHECK(le_stepped_##E, p, q, ((void *) p, (void *) q));  \
                CHECK(eq_derived_##E, p, k, ((void *) p, k, (void *) q)); \
            }                                                           \
        }                                                               \
    }

POINTEES(TEST_POINTER)

/* The subscripts of one element type and one index type, at every
   index its type holds within the reach, on B and on B + 5, and the
   local arrays.  The call's result is the index with stale bits above
   its type's width. */
#define TEST_SUBSCRIPT(E, T, S, I, N)                                   \
    for (int v = 0; v < INDEX_COUNT; ++v) {                             \
        const long i = (long) (I) indexes[v];                           \
                                                                        \
        if (i < -REACH || i > REACH)                                    \
            continue;                                                   \
                                                                        \
        const long s = stale(i, (int) sizeof(I));                       \
                                                                        \
        for (int d = 0; d < 2; ++d) {                                   \
            const long x = values[(v * 5 + d * 17) % COUNT];            \
            const int at = d ? 5 : 0;                                   \
                                                                        \
            MCHECK(get_##E##_##N, i, x, at, (P, (I) i));                \
            MCHECK(rget_##E##_##N, i, x, at, (P, (I) i));               \
            VCHECK(put_##E##_##N, i, x, at, (P, (I) i, x));             \
            MCHECK(putv_##E##_##N, i, x, at, (P, (I) i, x));            \
            MCHECK(addto_##E##_##N, i, x, at, (P, (I) i, (T) x));       \
            VCHECK(addon_##E##_##N, i, x, at, (P, (I) i, (T) x));       \
            VCHECK(mulon_##E##_##N, i, x, at, (P, (I) i, (T) x));       \
            MCHECK(inc_##E##_##N, i, x, at, (P, (I) i));                \
            MCHECK(postdec_##E##_##N, i, x, at, (P, (I) i));            \
            VCHECK(steps_##E##_##N, i, x, at, (P, (I) i));              \
            PCHECK(addr_##E##_##N, i, x, at, (P, (I) i));               \
            PCHECK(addr_star_##E##_##N, i, x, at, (P, (I) i));          \
            MCHECK(cget_##E##_##N, i, s, at, (P, s));                   \
            VCHECK(cput_##E##_##N, i, s, at, (P, s, x));                \
            PCHECK(caddr_##E##_##N, i, s, at, (P, s));                  \
                                                                        \
            if (i >= 0 && i < 16) {                                     \
                CHECK(aget_##E##_##N, i, x, ((I) i, x));                \
                CHECK(aaddr_##E##_##N, i, 0, ((I) i));                  \
            }                                                           \
        }                                                               \
    }
#define TEST_SUBSCRIPT_ALL(E, T, S)                                     \
    static void test_subscript_##E(void)                                \
    {                                                                   \
        INDEXES(TEST_SUBSCRIPT, E, T, S)                                \
    }

ELEMENTS(TEST_SUBSCRIPT_ALL)

/* the constant subscripts and displacements of one element type, on
   B and on B + 3, over the widest span */
#define TEST_KSUB(E, T, S, name, c)                                     \
    MCHECK(kget_##E##_##name, x, at, at, ((T *) P));                    \
    VCHECK(kput_##E##_##name, x, at, at, ((T *) P, x));                 \
    MCHECK(kinc_##E##_##name, x, at, at, ((T *) P));

#define TEST_KBYTE(E, T, S, name, d)                                    \
    MCHECK(kview_##E##_##name, x, at, at, ((char *) P));                \
    VCHECK(kview_put_##E##_##name, x, at, at, ((char *) P, x));         \
    MCHECK(kview_index_##E##_##name, x, at, at, ((char *) P));

#define TEST_CONSTANTS(E, T, S)                                         \
    static void test_constants_##E(void)                                \
    {                                                                   \
        span = WIDEST;                                                  \
                                                                        \
        for (int k = 0; k < 2; ++k) {                                   \
            const long x = values[k * 17 + 11];                         \
            const int at = k * 3;                                       \
                                                                        \
            KSUBS(TEST_KSUB, E, T, S)                                   \
            KBYTES(TEST_KBYTE, E, T, S)                                 \
        }                                                               \
                                                                        \
        span = SPAN;                                                    \
    }

ELEMENTS(TEST_CONSTANTS)

/* the walks, the pointers in memory, and the arrays of one scalar
   pointee */
#define TEST_WALKS(E, T, S)                                             \
    static void test_walks_##E(void)                                    \
    {                                                                   \
        for (int i = 0; i < COUNT; ++i) {                               \
            const long x = values[i];                                   \
            const long n = values[(i * 3 + 1) % COUNT];                 \
            const int at = i % 8;                                       \
            const int count = i % 17;                                   \
                                                                        \
            VCHECK(cell_steps_##E, x, 0, at, ((T **) C));               \
            PCHECK(cell_post_##E, x, 0, at, ((T **) C));                \
            PCHECK(cell_pre_##E, x, 0, at, ((T **) C + 3));             \
            PCHECK(cell_addeq_##E, x, n, at, ((T **) C + 5, n));        \
            MCHECK(walk_##E, x, 0, at, ((T *) P, x));                   \
            MCHECK(walk_back_##E, x, 0, at, ((T *) P));                 \
            MCHECK(sum_##E, x, count, at, ((T *) P, (T *) P + count));  \
            MCHECK(sum_back_##E, x, count, at,                          \
                   ((T *) P - count, (T *) P));                         \
            MCHECK(fill_##E, x, count, at, ((T *) P, (T *) P + count, x)); \
            MCHECK(pp_##E, i % CELLS, i % 5, at,                        \
                   ((T **) C, i % CELLS, i % 5));                       \
            VCHECK(pp_put_##E, i % CELLS, i % 5, at,                    \
                   ((T **) C, i % CELLS, i % 5, x));                    \
                                                                        \
            CHECK(mat_##E, i % 4, i % 5, (i % 4, i % 5, x));            \
            MCHECK(pmat_##E, i % 4, i % 5, at, (P, i % 4, i % 5));      \
            VCHECK(pmat_put_##E, i % 4, i % 5, at,                      \
                   (P, i % 4 - 1, i % 5, x));                           \
            PCHECK(prow_##E, i % 4, 0, at, (P, i % 4 - 2));             \
            CHECK(loop_index_##E, i % 65, x, (i % 65, x));              \
            CHECK(loop_walk_##E, i % 65, x, (i % 65, x));               \
            CHECK(loop_walk_##E, 64 - i % 65, x, (64 - i % 65, x));     \
            CHECK(ends_##E, 0, x, (0, x));                              \
            CHECK(ends_##E, 4999, x, (4999, x));                        \
            CHECK(vast_##E, 0, x, (0, x));                              \
            CHECK(vast_##E, 69999, x, (69999, x));                      \
        }                                                               \
    }

SCALARS(TEST_WALKS)

/* the literals, depth, and the indexes that are calls */
static void
test_rest(void)
{
    for (int i = 0; i < 7; ++i)
        CHECK(lit_index, i, 0, (i));

    for (int i = 0; i < 8; ++i)
        CHECK(lit_rindex, i, 0, (i));

    CHECK(lit_walk, 0, 0, ());
    CHECK(lit_length, 0, 0, ());

    for (int i = 0; i < COUNT; ++i) {
        for (int k = 1; k < COUNT; k += 3) {
            long v[8];

            for (int p = 0; p < 8; ++p)
                v[p] = values[(i + p * k) % COUNT];

            const long x = v[0];
            const int at = (i + k) % 8;
            const int j = (i + k) % 17 - 8;

            MCHECK(deep_index, x, j, at,
                   (v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7],
                    (long *) P, j));
            MCHECK(deep_put, x, j, at,
                   (v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7],
                    (int *) P, j));
            MCHECK(deep_addr, x, j, at,
                   (v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7],
                    (short *) P, j));
            MCHECK(deep_diff, x, j, at,
                   (v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7],
                    (int *) P, (int *) P - j));
            MCHECK(call_index, x, j, at, ((long *) P, j));
            MCHECK(call_put, x, j, at, ((int *) P, j, v[1]));
            CHECK(call_local, i % 8, x, (i % 8, x));
            PCHECK(call_add, x, v[1], at, ((short *) P, v[1]));
        }
    }
}

int
main(void)
{
    if (getenv("PROBE_ALL"))
        shown = LONG_MAX;

#define RUN_POINTEE(E, T, S) test_arith_##E(); test_pointer_##E();
    POINTEES(RUN_POINTEE)

#define RUN_ELEMENT(E, T, S) test_subscript_##E(); test_constants_##E();
    ELEMENTS(RUN_ELEMENT)

#define RUN_SCALAR(E, T, S) test_walks_##E();
    SCALARS(RUN_SCALAR)

    test_rest();

    printf("%ld checks, %ld differ\n", total, bad);

    return bad != 0;
}
