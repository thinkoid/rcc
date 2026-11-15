/* The statics probe's main, compiled by clang -O0: calls every function
   of statics.c as rcc compiled it and as clang did (ref_), and compares
   each result with what it should be.  Prints the first forty
   differences, or every one when PROBE_ALL is set in the environment,
   and a summary; exits 0 when nothing differs, 1 otherwise.

   The objects of statics.c are static, so the main names none of
   them: it reads and writes them through statics.c's functions.  Each
   build has objects of its own, and a function of one build reaches
   its own build's alone.  The initialized objects of both builds are
   checked first, before anything writes them, against their
   initializers, which the main converts to the object's type itself,
   and the others against zero; then each object is written over a
   table of values and read back, a block's object through the value
   it returns at the next call, so an object two functions or two
   blocks shared would show.  An address is compared with its own
   build's.

   The main defines a global object and a global function of the names
   statics.c and statics-lib.c each give a static of their own, so a
   .globl on either file's would not link, and checks that each of the
   three reads its own. */

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include "statics.h"

static const long values[] = { 0,
                               1,
                               -1,
                               2,
                               127,
                               128,
                               255,
                               256,
                               -129,
                               32767,
                               -32769,
                               65535,
                               65536,
                               0x7fffffff,
                               0x80000000L,
                               0xffffffffL,
                               0x100000000L,
                               0x123456789abcdef0L,
                               LONG_MAX,
                               LONG_MIN };

#define COUNT ((int) (sizeof values / sizeof *values))

static long total;
static long bad;
static long shown = 40;

/* a global of each name statics.c and statics-lib.c give a static */
int shared = 3;

int
same_name(void)
{
        return 30;
}

/* declarations: each function and its reference */
#define BOTH(R, f, parameters) \
        R f parameters;        \
        R ref_##f parameters;

#define DECLARE(T, N, V)                    \
        BOTH(long, read_f_##N, (void) )     \
        BOTH(void, write_f_##N, (long) )    \
        BOTH(long, add_f_##N, (long) )      \
        BOTH(long, read_fi_##N, (void) )    \
        BOTH(T *, address_fi_##N, (void) )  \
        BOTH(T *, pointer_fpi_##N, (void) ) \
        BOTH(long, deref_fpi_##N, (void) )  \
        BOTH(int, null_fp_##N, (void) )     \
        BOTH(void, set_fp_##N, (T *) )      \
        BOTH(long, store_fp_##N, (long) )   \
        BOTH(long, swap_b_##N, (long) )     \
        BOTH(long, step_bi_##N, (void) )    \
        BOTH(T *, keep_bp_##N, (T *) )      \
        BOTH(long, block_pointer_##N, (long) )

INITIAL(DECLARE)

BOTH(long, file_table, (int) )
BOTH(long, file_grid, (int, int) )
BOTH(long, file_word, (int) )
BOTH(long, file_exact, (int) )
BOTH(long, file_sized, (int) )
BOTH(long, file_name, (int) )
BOTH(long, file_names, (int, int) )
BOTH(long, file_braced, (void) )
BOTH(long, file_empty, (int) )
BOTH(long, file_element, (void) )
BOTH(long, file_suffix, (int) )
BOTH(long, file_sizes, (void) )
BOTH(long, block_table, (int) )
BOTH(long, block_grid, (int, int) )
BOTH(long, block_word, (int) )
BOTH(long, block_exact, (int) )
BOTH(long, block_names, (int, int) )
BOTH(long, block_braced, (void) )
BOTH(long, block_empty, (int) )
BOTH(long, block_element, (void) )
BOTH(long, block_sizes, (void) )
BOTH(int, counter, (void) )
BOTH(int, first, (int) )
BOTH(int, second, (int) )
BOTH(int, blocks, (int, int) )
BOTH(int, file_shade, (void) )
BOTH(int, hides, (void) )
BOTH(long, call_helper, (long) )
BOTH(long, call_through, (long) )
BOTH(helper_t, helper_address, (void) )
BOTH(int, same_helper, (void) )
BOTH(int, own_shared, (void) )
BOTH(int, own_same_name, (void) )
BOTH(int, lib_shared, (void) )
BOTH(int, lib_same_name, (void) )
BOTH(long, read_again, (void) )

static void
report(const char *name, long got, long want)
{
        ++total;

        if (got == want)
                return;

        if (++bad <= shown)
                printf("%s: got %ld, want %ld\n", name, got, want);
}

/* one check of both builds: rcc's value and the reference's, each
   against what it should be */
#define PAIR(name, rcc, reference, want)           \
        report(name, (long) (rcc), (long) (want)), \
            report("ref_" name, (long) (reference), (long) (want))

/* the same call of both builds */
#define CALLS(f, arguments, want) PAIR(#f, f arguments, ref_##f arguments, want)

/* The objects of each type before anything writes them: the zero ones
   zero, the initialized ones their initializer converted, the pointer
   to fi_N its build's own fi_N.  A block's initialized object is read
   by its first call. */
#define INITIAL_CHECKS(T, N, V)                                               \
        CALLS(read_f_##N, (), 0);                                             \
        CALLS(read_fi_##N, (), (T) V);                                        \
        CALLS(deref_fpi_##N, (), (T) V);                                      \
        CALLS(null_fp_##N, (), 1);                                            \
        report("pointer_fpi_" #N, pointer_fpi_##N() == address_fi_##N(), 1);  \
        report("ref_pointer_fpi_" #N,                                         \
               ref_pointer_fpi_##N() == ref_address_fi_##N(), 1);             \
        CALLS(swap_b_##N, (0), 0);                                            \
        CALLS(step_bi_##N, (), (T) V);                                        \
        PAIR("keep_bp_" #N, keep_bp_##N(0) == 0, ref_keep_bp_##N(0) == 0, 1); \
        CALLS(block_pointer_##N, ((T) V), (T) V);

static void
test_initial(void)
{
        INITIAL(INITIAL_CHECKS)

        for (int i = 0; i < 3; ++i)
                CALLS(file_table, (i), 1 + i);

        static const int want_grid[2][3] = { { 1, 0, 0 }, { 4, 5, 0 } };
        static const int want_block_grid[2][3] = { { 1, 2, 3 }, { 0, 0, 0 } };

        for (int i = 0; i < 2; ++i)
                for (int j = 0; j < 3; ++j) {
                        CALLS(file_grid, (i, j), want_grid[i][j]);
                        CALLS(block_grid, (i, j), want_block_grid[i][j]);
                }

        for (int i = 0; i < 8; ++i)
                CALLS(file_word, (i), i < 3 ? "abc"[i] : 0);

        for (int i = 0; i < 3; ++i)
                CALLS(file_exact, (i), "abc"[i]);

        for (int i = 0; i < 5; ++i)
                CALLS(file_sized, (i), "abcd"[i]);

        for (int i = 0; i < 4; ++i) {
                CALLS(file_name, (i), "Jan"[i]);
                CALLS(file_names, (0, i), "Feb"[i]);
                CALLS(file_names, (1, i), "Mar"[i]);
                CALLS(block_names, (0, i), "May"[i]);
                CALLS(block_names, (1, i), "Jun"[i]);
                CALLS(block_names, (2, i), "Jul"[i]);
        }

        for (int i = 0; i < 3; ++i)
                CALLS(file_suffix, (i), "pr"[i]);

        CALLS(file_braced, (), 5);
        CALLS(block_braced, (), -9);

        for (int i = 0; i < 3; ++i)
                CALLS(file_empty, (i), 0);

        for (int i = 0; i < 2; ++i)
                CALLS(block_empty, (i), 0);

        CALLS(file_element, (), 3);
        CALLS(block_element, (), 2);

        for (int i = 0; i < 4; ++i)
                CALLS(block_table, (i), 6 + i);

        for (int i = 0; i < 6; ++i)
                CALLS(block_word, (i), i < 3 ? "xyz"[i] : 0);

        for (int i = 0; i < 2; ++i)
                CALLS(block_exact, (i), "pq"[i]);

        CALLS(file_sizes, (), 12 * 100 + 5 * 10 + 16);
        CALLS(block_sizes, (), 16 * 10 + 3);
}

/* Each object of each type written over the values and read back: a
   file's by its reader, a block's by what its function returns at the
   next call, the value stored at the call before, converted. */
#define WRITE_CHECKS(T, N, V)                                      \
        for (int k = 0; k < COUNT; ++k) {                          \
                const long v = values[k];                          \
                const long before = k > 0 ? (T) values[k - 1] : 0; \
                                                                   \
                write_f_##N(v);                                    \
                ref_write_f_##N(v);                                \
                CALLS(read_f_##N, (), (T) v);                      \
                CALLS(add_f_##N, (3), (T) ((T) v + 3));            \
                CALLS(swap_b_##N, (v), before);                    \
                CALLS(block_pointer_##N, (v),                      \
                      k > 0 ? (T) values[k - 1] : (T) V);          \
                CALLS(step_bi_##N, (), (T) ((T) V + 1 + k));       \
        }                                                          \
                                                                   \
        {                                                          \
                T rcc_cell = 0, ref_cell = 0;                      \
                                                                   \
                set_fp_##N(&rcc_cell);                             \
                ref_set_fp_##N(&ref_cell);                         \
                CALLS(store_fp_##N, (0x1234), (T) 0x1234);         \
                PAIR("cell_" #N, rcc_cell, ref_cell, (T) 0x1234);  \
                CALLS(null_fp_##N, (), 0);                         \
                                                                   \
                PAIR("keep_bp_" #N, keep_bp_##N(&rcc_cell) == 0,   \
                     ref_keep_bp_##N(&ref_cell) == 0, 1);          \
                PAIR("keep_bp_" #N, keep_bp_##N(0) == &rcc_cell,   \
                     ref_keep_bp_##N(0) == &ref_cell, 1);          \
        }

static void
test_writes(void)
{
        INITIAL(WRITE_CHECKS)
}

/* A block's object counting its function's calls, and the four objects
   of one name, written in turn: each returns what it alone was given. */
static void
test_blocks(void)
{
        for (int n = 1; n <= 5; ++n)
                CALLS(counter, (), n);

        CALLS(first, (11), 0);
        CALLS(second, (12), 1000);
        CALLS(blocks, (1, 13), 2000);
        CALLS(blocks, (0, 14), 0);

        for (int k = 0; k < 4; ++k) {
                CALLS(first, (21 + 10 * k), 11 + 10 * k);
                CALLS(second, (22 + 10 * k), 12 + 10 * k);
                CALLS(blocks, (1, 23 + 10 * k), 13 + 10 * k);
                CALLS(blocks, (0, 24 + 10 * k), 14 + 10 * k);
        }

        CALLS(file_shade, (), 1);

        for (int k = 0; k < 3; ++k)
                CALLS(hides, (), ((2 + k) * 10 + 3 + k) * 10 + 2 + k);

        CALLS(file_shade, (), 1);
}

/* The static function by name, through a pointer and through the
   address it returns, which is its build's handler's. */
static void
test_functions(void)
{
        for (long x = -3; x <= 3; ++x) {
                CALLS(call_helper, (x), x * 3 + 1);
                CALLS(call_through, (x), 2 * (x * 3 + 1));
                PAIR("helper_address", helper_address()(x),
                     ref_helper_address()(x), x * 3 + 1);
        }

        CALLS(same_helper, (), 1);
        report("helper_address as ref_helper_address",
               helper_address() != ref_helper_address(), 1);
}

/* Each file's own static of a shared name, the main's global of it,
   and the names declared extern after static. */
static void
test_names(void)
{
        CALLS(own_shared, (), 1);
        CALLS(own_same_name, (), 10);
        CALLS(lib_shared, (), 2);
        CALLS(lib_same_name, (), 20);
        report("shared", shared, 3);
        report("same_name", same_name(), 30);

        CALLS(read_again, (), 4 + 8);
}

int
main(void)
{
        if (getenv("PROBE_ALL"))
                shown = LONG_MAX;

        test_initial();
        test_writes();
        test_blocks();
        test_functions();
        test_names();

        printf("%ld checks, %ld differ\n", total, bad);

        return bad != 0;
}
