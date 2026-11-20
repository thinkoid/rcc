/* The braces probe's main, compiled by clang -O0: checks the objects
   braces.c defines, as rcc compiled them and as clang did (ref_), and
   calls the functions of both builds.  Prints the first forty
   differences, or every one when PROBE_ALL is set in the environment,
   and a summary; exits 0 when nothing differs, 1 otherwise.

   Each array is checked element by element, in both builds, against
   the list it should hold, written out here with every element the
   list leaves out as its zero, as 6.7.11 and its examples give it;
   then rcc's bytes against the reference's.  The reference build of
   the same source checks the brace elision by itself: clang applied
   6.7.11p21 to the same list, so rcc's bytes equal to the reference's
   are rcc's elision equal to clang's.  The guard after each array
   holds GUARD in both builds, and in rcc's its label is at the
   array's end, since rcc writes the objects in the order of their
   definitions and a guard's alignment is no more than its array's: an
   array written one element short or long moves it.  An address
   differs between the builds, so a pointer is checked against the
   address of its own build's object.

   The objects are checked first, before anything writes them, and
   nothing here writes them at all; then each build's code reads them
   and gives the sizes the lists completed, and rcc's code reads the
   objects clang initialized in braces-lib.c. */

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "braces.h"

static long total;
static long bad;
static long shown = 40;

/* declarations: each object and its reference */
#define OBJECT(T, name) extern T name, ref_##name

OBJECT(int, n);
OBJECT(int, g_counted);
OBJECT(int, g_padded);
OBJECT(int, g_trailing);
OBJECT(int, g_trailing_counted);
OBJECT(int, g_empty);
OBJECT(int, g_big);
OBJECT(char, g_narrow);
OBJECT(char, g_bytes);
OBJECT(short, g_shorts);
OBJECT(short, g_ushorts);
OBJECT(long, g_longs);
OBJECT(long, g_ulongs);
OBJECT(int, g_full);
OBJECT(int, g_flat);
OBJECT(int, g_partly);
OBJECT(int, g_rows_counted);
OBJECT(int, g_y);
OBJECT(int, g_z);
OBJECT(short, g_q);
OBJECT(short, g_q_flat);
OBJECT(short, g_q_full);
OBJECT(int, g_scalar_braces);
OBJECT(int, g_deep);
OBJECT(long, g_names);
OBJECT(long, g_pointers);
OBJECT(long, g_functions);
OBJECT(long, g_rows);
OBJECT(long, g_row_pointers);
OBJECT(long, g_suffixes);
OBJECT(char, g_months);
OBJECT(char, g_letters);
OBJECT(char, g_exact);
OBJECT(char, g_padded_words);
OBJECT(char, g_words);
OBJECT(char, g_spelled);
OBJECT(char, g_braced_string);
OBJECT(char, g_braced_words);
OBJECT(int, scalar);
OBJECT(int, scalar_comma);
OBJECT(int, scalar_empty);
OBJECT(long, scalar_long);
OBJECT(char_pointer_t, string_pointer);
OBJECT(int_pointer_t, pointer_empty);

#define ARRAY_OBJECT(T, name, dimensions) \
        extern T name dimensions, ref_##name dimensions

ARRAY_OBJECT(int, target, [4]);
ARRAY_OBJECT(int, counted, [3]);
ARRAY_OBJECT(int, padded, [5]);
ARRAY_OBJECT(int, trailing, [3]);
ARRAY_OBJECT(int, trailing_counted, [2]);
ARRAY_OBJECT(int, empty, [3]);
ARRAY_OBJECT(int, big, [8]);
ARRAY_OBJECT(signed char, narrow, [4]);
ARRAY_OBJECT(unsigned char, bytes, [3]);
ARRAY_OBJECT(short, shorts, [3]);
ARRAY_OBJECT(unsigned short, ushorts, [2]);
ARRAY_OBJECT(long, longs, [3]);
ARRAY_OBJECT(unsigned long, ulongs, [2]);
ARRAY_OBJECT(int, full, [ROWS][COLUMNS]);
ARRAY_OBJECT(int, flat, [ROWS][COLUMNS]);
ARRAY_OBJECT(int, partly, [ROWS][COLUMNS]);
ARRAY_OBJECT(int, rows_counted, [2][COLUMNS]);
ARRAY_OBJECT(int, y, [4][3]);
ARRAY_OBJECT(int, z, [4][3]);
ARRAY_OBJECT(short, q, [4][3][2]);
ARRAY_OBJECT(short, q_flat, [4][3][2]);
ARRAY_OBJECT(short, q_full, [4][3][2]);
ARRAY_OBJECT(int, scalar_braces, [ROWS][COLUMNS]);
ARRAY_OBJECT(int, deep, [2][2][2]);
ARRAY_OBJECT(char_pointer_t, names, [3]);
ARRAY_OBJECT(int_pointer_t, pointers, [6]);
ARRAY_OBJECT(unary_t, functions, [3]);
ARRAY_OBJECT(int_pointer_t, rows, [2]);
ARRAY_OBJECT(row_pointer_t, row_pointers, [3]);
ARRAY_OBJECT(char_pointer_t, suffixes, [2][2]);
ARRAY_OBJECT(char, months, [3][4]);
ARRAY_OBJECT(char, letters, [ROWS][4]);
ARRAY_OBJECT(char, exact, [2][3]);
ARRAY_OBJECT(char, padded_words, [3][6]);
ARRAY_OBJECT(char, words, [2][2][4]);
ARRAY_OBJECT(char, spelled, [2][3]);
ARRAY_OBJECT(char, braced_string, [4]);
ARRAY_OBJECT(char, braced_words, [2][3]);

/* declarations: each function and its reference */
#define BOTH(R, f, parameters) \
        R f parameters;        \
        R ref_##f parameters;

BOTH(int, twice, (int))
BOTH(int, lib_triple, (int))
BOTH(long, size_counted, (void))
BOTH(long, size_trailing_counted, (void))
BOTH(long, size_narrow, (void))
BOTH(long, size_rows_counted, (void))
BOTH(long, size_names, (void))
BOTH(long, size_pointers, (void))
BOTH(long, size_months, (void))
BOTH(long, size_braced_string, (void))
BOTH(long, size_braced_words, (void))
BOTH(long, read_partly, (int, int))
BOTH(long, read_q, (int, int, int))
BOTH(long, read_ulongs, (int))
BOTH(long, read_names, (int, int))
BOTH(long, read_months, (int, int))
BOTH(long, read_suffixes, (int, int, int))
BOTH(long, call_function, (int, int))
BOTH(long, through_pointer, (int))
BOTH(long, through_row, (int, int))
BOTH(long, through_row_pointer, (int, int))
BOTH(int, is_pointers, (void))
BOTH(int, is_functions, (void))
BOTH(int, is_rows, (void))
BOTH(long, read_lib_table, (int, int))
BOTH(long, read_lib_elided, (int, int))
BOTH(long, read_lib_names, (int, int))
BOTH(long, read_lib_words, (int, int))

/* the lists the arrays should hold, every element written out */
static const int want_counted[] = { 1, 2, 3 };
static const int want_padded[] = { 1, 2, 0, 0, 0 };
static const int want_trailing[] = { 4, 5, 6 };
static const int want_trailing_counted[] = { 7, 8 };
static const int want_empty[] = { 0, 0, 0 };
static const int want_big[] = { 1, 0, 0, 0, 0, 0, 0, 0 };
static const signed char want_narrow[] = { -1, 127, -128, 44 };
static const unsigned char want_bytes[] = { 255, 128, 255 };
static const short want_shorts[] = { -3, -25536, 0 };
static const unsigned short want_ushorts[] = { 65535, 65534 };
static const long want_longs[] = { -1, 1099511627776, 97 };
static const unsigned long want_ulongs[] = { ULONG_MAX, 0 };
static const int want_full[] = { 1, 2, 3, 4, 5, 6 };
static const int want_partly[] = { 1, 0, 0, 4, 5, 0 };
static const int want_rows_counted[] = { 1, 2, 3, 4, 0, 0 };
static const int want_y[] = { 1, 3, 5, 2, 4, 6, 3, 5, 7, 0, 0, 0 };
static const int want_z[] = { 1, 0, 0, 2, 0, 0, 3, 0, 0, 4, 0, 0 };
static const short want_q[] = { 1, 0, 0, 0, 0, 0, 2, 3, 0, 0, 0, 0,
                                4, 5, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
static const int want_scalar_braces[] = { 1, 2, 0, 0, 0, 0 };
static const int want_deep[] = { 1, 2, 3, 0, 4, 0, 0, 0 };
static const char want_months[] = "Jan\0Feb\0Mar";
static const char want_letters[] = "Jan\0Feb";
static const char want_exact[] = "abcde";
static const char want_padded_words[18] = "ab";
static const char want_words[16] = "ab\0\0cd\0\0ef";
static const char want_spelled[6] = "abcd";
static const char want_braced_string[] = "abc";
static const char want_braced_words[] = "ab\0c\0";
static const char *const want_names[] = { "Jan", "Feb", "Mar" };

static void
report(const char *name, long got, long want)
{
        ++total;

        if (got == want)
                return;

        if (++bad <= shown)
                printf("%s: got %ld, want %ld\n", name, got, want);
}

static void
report_element(const char *name, long i, long got, long want)
{
        ++total;

        if (got == want)
                return;

        if (++bad <= shown)
                printf("%s[%ld]: got %ld, want %ld\n", name, i, got, want);
}

/* one check of both builds: rcc's value and the reference's, each
   against what it should be */
#define PAIR(name, rcc, reference, want)           \
        report(name, (long) (rcc), (long) (want)), \
            report("ref_" name, (long) (reference), (long) (want))

/* the element i, of width bytes, of the array at base, read as a
   signed integer of its width */
static long
element(const void *base, long i, long width)
{
        const char *p = (const char *) base + i * width;

        switch (width) {
        case 1: return *(const signed char *) p;
        case 2: return *(const short *) p;
        case 4: return *(const int *) p;
        default: return *(const long *) p;
        }
}

/* each element of an array of both builds, of size bytes, against the
   list want, and rcc's bytes against the reference's */
static void
elements(const char *name,
         const void *rcc,
         const void *reference,
         const void *want,
         long size,
         long width)
{
        char ref_name[64];

        snprintf(ref_name, sizeof ref_name, "ref_%s", name);

        for (long i = 0; i < size / width; ++i) {
                report_element(name, i, element(rcc, i, width),
                               element(want, i, width));
                report_element(ref_name, i, element(reference, i, width),
                               element(want, i, width));
        }

        snprintf(ref_name, sizeof ref_name, "%s as ref_%s", name, name);
        report(ref_name, memcmp(rcc, reference, size) == 0, 1);
}

/* an array of both builds against its list, which has its size */
#define ELEMENTS(name, want)                                         \
        do {                                                         \
                _Static_assert(sizeof name == sizeof want, #name);   \
                elements(#name, name, ref_##name, want, sizeof name, \
                         sizeof *want);                              \
        } while (0)

/* the guard after an array in both builds, and in rcc's its label at
   the array's end */
#define GUARDED(name)                                                \
        PAIR("g_" #name, g_##name, ref_g_##name, GUARD),             \
            report(#name " end", (char *) &g_##name - (char *) name, \
                   (long) sizeof name)

/* Arrays of integers of one dimension, each width: the size from the
   count, the rest zero, a trailing comma, the empty initializer, each
   element converted to its type. */
static void
test_integers(void)
{
        ELEMENTS(counted, want_counted);
        GUARDED(counted);
        ELEMENTS(padded, want_padded);
        GUARDED(padded);
        ELEMENTS(trailing, want_trailing);
        GUARDED(trailing);
        ELEMENTS(trailing_counted, want_trailing_counted);
        GUARDED(trailing_counted);
        ELEMENTS(empty, want_empty);
        GUARDED(empty);
        ELEMENTS(big, want_big);
        GUARDED(big);
        ELEMENTS(narrow, want_narrow);
        GUARDED(narrow);
        ELEMENTS(bytes, want_bytes);
        GUARDED(bytes);
        ELEMENTS(shorts, want_shorts);
        GUARDED(shorts);
        ELEMENTS(ushorts, want_ushorts);
        GUARDED(ushorts);
        ELEMENTS(longs, want_longs);
        GUARDED(longs);
        ELEMENTS(ulongs, want_ulongs);
        GUARDED(ulongs);

#if defined(__x86_64__)
        /* the psABI aligns an array of 16 bytes or more to 16 */
        report("big alignment", (long) ((unsigned long) big % 16), 0);
#endif
}

/* Arrays of arrays, braced in full, flat and in part: each against the
   list written out, and the elision against the reference's. */
static void
test_aggregates(void)
{
        ELEMENTS(full, want_full);
        GUARDED(full);
        ELEMENTS(flat, want_full);
        GUARDED(flat);
        ELEMENTS(partly, want_partly);
        GUARDED(partly);
        ELEMENTS(rows_counted, want_rows_counted);
        GUARDED(rows_counted);
        ELEMENTS(y, want_y);
        GUARDED(y);
        ELEMENTS(z, want_z);
        GUARDED(z);
        ELEMENTS(q, want_q);
        GUARDED(q);
        ELEMENTS(q_flat, want_q);
        GUARDED(q_flat);
        ELEMENTS(q_full, want_q);
        GUARDED(q_full);
        ELEMENTS(scalar_braces, want_scalar_braces);
        GUARDED(scalar_braces);
        ELEMENTS(deep, want_deep);
        GUARDED(deep);
}

/* Arrays of pointers: each element against the address of its own
   build's object, or what it designates. */
static void
test_pointers(void)
{
        for (int i = 0; i < 3; ++i)
                PAIR("names", strcmp(names[i], want_names[i]),
                     strcmp(ref_names[i], want_names[i]), 0);
        GUARDED(names);

        PAIR("pointers[0]", pointers[0] == &n, ref_pointers[0] == &ref_n, 1);
        PAIR("pointers[1]", pointers[1] - target, ref_pointers[1] - ref_target,
             2);
        PAIR("pointers[2]", pointers[2] - target, ref_pointers[2] - ref_target,
             3);
        PAIR("pointers[3]", pointers[3] == 0, ref_pointers[3] == 0, 1);
        PAIR("pointers[4]", pointers[4] == 0, ref_pointers[4] == 0, 1);
        PAIR("pointers[5]", (long) pointers[5], (long) ref_pointers[5], 4096);
        GUARDED(pointers);

        PAIR("functions[0]", functions[0] == twice,
             ref_functions[0] == ref_twice, 1);
        PAIR("functions[1]", functions[1] == lib_triple,
             ref_functions[1] == ref_lib_triple, 1);
        PAIR("functions[2]", functions[2] == 0, ref_functions[2] == 0, 1);
        GUARDED(functions);

        PAIR("rows[0]", rows[0] == full[1], ref_rows[0] == ref_full[1], 1);
        PAIR("rows[1]", rows[1] == flat[0], ref_rows[1] == ref_flat[0], 1);
        GUARDED(rows);

        PAIR("row_pointers[0]", row_pointers[0] == &full[1],
             ref_row_pointers[0] == &ref_full[1], 1);
        PAIR("row_pointers[1]", row_pointers[1] == &partly[0],
             ref_row_pointers[1] == &ref_partly[0], 1);
        PAIR("row_pointers[2]", row_pointers[2] == &partly[1],
             ref_row_pointers[2] == &ref_partly[1], 1);
        GUARDED(row_pointers);

        PAIR("suffixes[0][0]", strcmp(suffixes[0][0], "b"),
             strcmp(ref_suffixes[0][0], "b"), 0);
        PAIR("suffixes[0][1]", suffixes[0][1] == 0, ref_suffixes[0][1] == 0, 1);
        PAIR("suffixes[1][0]", strcmp(suffixes[1][0], "cd"),
             strcmp(ref_suffixes[1][0], "cd"), 0);
        PAIR("suffixes[1][1]", strcmp(suffixes[1][1], "ef"),
             strcmp(ref_suffixes[1][1], "ef"), 0);
        GUARDED(suffixes);
}

/* Arrays of char: a string for an element, by the element's own
   bytes, and the elements spelled one by one. */
static void
test_chars(void)
{
        ELEMENTS(months, want_months);
        GUARDED(months);
        ELEMENTS(letters, want_letters);
        GUARDED(letters);
        ELEMENTS(exact, want_exact);
        GUARDED(exact);
        ELEMENTS(padded_words, want_padded_words);
        GUARDED(padded_words);
        ELEMENTS(words, want_words);
        GUARDED(words);
        ELEMENTS(spelled, want_spelled);
        GUARDED(spelled);
        ELEMENTS(braced_string, want_braced_string);
        GUARDED(braced_string);
        ELEMENTS(braced_words, want_braced_words);
        GUARDED(braced_words);
}

/* Scalars in braces, and the empty initializer. */
static void
test_scalars(void)
{
        PAIR("scalar", scalar, ref_scalar, 5);
        PAIR("scalar_comma", scalar_comma, ref_scalar_comma, 6);
        PAIR("scalar_empty", scalar_empty, ref_scalar_empty, 0);
        PAIR("scalar_long", scalar_long, ref_scalar_long, -2);
        PAIR("string_pointer", strcmp(string_pointer, "abc"),
             strcmp(ref_string_pointer, "abc"), 0);
        PAIR("pointer_empty", pointer_empty == 0, ref_pointer_empty == 0, 1);
}

/* The sizes each build's code gives the arrays the lists complete. */
static void
test_sizes(void)
{
        PAIR("size_counted", size_counted(), ref_size_counted(),
             3 * sizeof(int));
        PAIR("size_trailing_counted", size_trailing_counted(),
             ref_size_trailing_counted(), 2 * sizeof(int));
        PAIR("size_narrow", size_narrow(), ref_size_narrow(), 4);
        PAIR("size_rows_counted", size_rows_counted(), ref_size_rows_counted(),
             2 * COLUMNS * sizeof(int));
        PAIR("size_names", size_names(), ref_size_names(), 3 * sizeof(char *));
        PAIR("size_pointers", size_pointers(), ref_size_pointers(),
             6 * sizeof(int *));
        PAIR("size_months", size_months(), ref_size_months(), 12);
        PAIR("size_braced_string", size_braced_string(),
             ref_size_braced_string(), 4);
        PAIR("size_braced_words", size_braced_words(), ref_size_braced_words(),
             6);
}

/* The objects read by each build's code, and the comparisons each
   build's code makes of the addresses in its data with those it
   forms. */
static void
test_code(void)
{
        for (int i = 0; i < ROWS; ++i)
                for (int j = 0; j < COLUMNS; ++j)
                        PAIR("read_partly", read_partly(i, j),
                             ref_read_partly(i, j),
                             want_partly[i * COLUMNS + j]);

        for (int i = 0; i < 4; ++i)
                for (int j = 0; j < 3; ++j)
                        for (int k = 0; k < 2; ++k)
                                PAIR("read_q", read_q(i, j, k),
                                     ref_read_q(i, j, k),
                                     want_q[(i * 3 + j) * 2 + k]);

        PAIR("read_ulongs", read_ulongs(0), ref_read_ulongs(0), 1);
        PAIR("read_ulongs", read_ulongs(1), ref_read_ulongs(1), 0);

        for (int i = 0; i < 3; ++i)
                for (int k = 0; k < 4; ++k) {
                        PAIR("read_names", read_names(i, k),
                             ref_read_names(i, k), want_names[i][k]);
                        PAIR("read_months", read_months(i, k),
                             ref_read_months(i, k), want_months[i * 4 + k]);
                }

        PAIR("read_suffixes", read_suffixes(0, 0, 0),
             ref_read_suffixes(0, 0, 0), 'b');
        PAIR("read_suffixes", read_suffixes(0, 0, 1),
             ref_read_suffixes(0, 0, 1), 0);
        PAIR("read_suffixes", read_suffixes(1, 0, 0),
             ref_read_suffixes(1, 0, 0), 'c');
        PAIR("read_suffixes", read_suffixes(1, 1, 1),
             ref_read_suffixes(1, 1, 1), 'f');

        PAIR("call_function", call_function(0, 21), ref_call_function(0, 21),
             42);
        PAIR("call_function", call_function(1, -7), ref_call_function(1, -7),
             -21);

        PAIR("through_pointer", through_pointer(0), ref_through_pointer(0), 7);
        PAIR("through_pointer", through_pointer(1), ref_through_pointer(1), 12);
        PAIR("through_pointer", through_pointer(2), ref_through_pointer(2), 13);

        for (int j = 0; j < COLUMNS; ++j) {
                PAIR("through_row", through_row(0, j), ref_through_row(0, j),
                     4 + j);
                PAIR("through_row", through_row(1, j), ref_through_row(1, j),
                     1 + j);
                PAIR("through_row_pointer", through_row_pointer(0, j),
                     ref_through_row_pointer(0, j), 4 + j);
                PAIR("through_row_pointer", through_row_pointer(1, j),
                     ref_through_row_pointer(1, j), want_partly[j]);
                PAIR("through_row_pointer", through_row_pointer(2, j),
                     ref_through_row_pointer(2, j), want_partly[COLUMNS + j]);
        }

        PAIR("is_pointers", is_pointers(), ref_is_pointers(), 1);
        PAIR("is_functions", is_functions(), ref_is_functions(), 1);
        PAIR("is_rows", is_rows(), ref_is_rows(), 1);
}

/* The objects braces-lib.c initializes, read by rcc's code and by the
   reference's. */
static void
test_library(void)
{
        static const int want_elided[] = { 7, 0, 0, 8, 9, 0 };

        for (int i = 0; i < ROWS; ++i)
                for (int j = 0; j < COLUMNS; ++j) {
                        PAIR("read_lib_table", read_lib_table(i, j),
                             ref_read_lib_table(i, j), 1 + i * COLUMNS + j);
                        PAIR("read_lib_elided", read_lib_elided(i, j),
                             ref_read_lib_elided(i, j),
                             want_elided[i * COLUMNS + j]);
                }

        for (int k = 0; k < 4; ++k) {
                PAIR("read_lib_names", read_lib_names(0, k),
                     ref_read_lib_names(0, k), "Apr"[k]);
                PAIR("read_lib_names", read_lib_names(1, k),
                     ref_read_lib_names(1, k), "May"[k]);
                PAIR("read_lib_words", read_lib_words(0, k),
                     ref_read_lib_words(0, k), "Jun"[k]);
                PAIR("read_lib_words", read_lib_words(1, k),
                     ref_read_lib_words(1, k), "Jul"[k]);
        }
}

int
main(void)
{
        if (getenv("PROBE_ALL"))
                shown = LONG_MAX;

        test_integers();
        test_aggregates();
        test_pointers();
        test_chars();
        test_scalars();
        test_sizes();
        test_code();
        test_library();

        printf("%ld checks, %ld differ\n", total, bad);

        return bad != 0;
}
