/* The initializers probe's main, compiled by clang -O0: checks the
   objects initializers.c defines, as rcc compiled them and as clang did
   (ref_), and calls the functions of both builds.  Prints the first
   forty differences, or every one when PROBE_ALL is set in the
   environment, and a summary; exits 0 when nothing differs, 1
   otherwise.

   Each build has objects of its own: those initializers.c defines are
   rcc's under their names and the reference's under the prefix, and
   initializers-lib.c is linked twice, the second copy renamed.  An
   address differs between the builds, so each check is of what an
   address designates in its own build: a pointer against the address
   of its build's object, the difference of two pointers, the bytes of
   a string, the result of a call through a function pointer, and the
   same comparisons made by each build's code.

   The objects an initializer writes are checked first, before anything
   writes them; then the arrays they point into are filled alike in
   both builds, and what the pointers designate is read through them,
   by the main and by each build's code. */

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "initializers.h"

static long total;
static long bad;
static long shown = 40;

/* declarations: each object and its reference */
#define OBJECT(T, name) extern T name, ref_##name

OBJECT(int, n);
OBJECT(int, signalled);
OBJECT(int, late);
OBJECT(int, after_tentative);
OBJECT(int_pointer_t, p_n);
OBJECT(int_pointer_t, p_element);
OBJECT(int_pointer_t, p_decay);
OBJECT(int_pointer_t, p_sum);
OBJECT(int_pointer_t, p_difference);
OBJECT(int_pointer_t, p_cell);
OBJECT(row_pointer_t, p_row);
OBJECT(int_pointer_t, p_row_decay);
OBJECT(char_pointer_t, p_bytes);
OBJECT(long_pointer_t, p_cast);
OBJECT(void_pointer_t, p_void);
OBJECT(int_pointer_pointer_t, p_pointer);
OBJECT(int_pointer_t, p_late);
OBJECT(int_pointer_t, p_object);
OBJECT(int_pointer_t, p_array);
OBJECT(handler_t, h_signal);
OBJECT(handler_t, h_signal_address);
OBJECT(unary_t, f_twice);
OBJECT(unary_t, f_triple);
OBJECT(thunk_t, h_cast);
OBJECT(char_pointer_t, s_word);
OBJECT(char_pointer_t, s_suffix);
OBJECT(char_pointer_t, s_element);
OBJECT(char_pointer_t, s_escaped);
OBJECT(int_pointer_t, p_fixed);
OBJECT(int_pointer_t, p_fixed_sum);
OBJECT(int_pointer_t, p_null);
OBJECT(int_pointer_t, p_void_null);
OBJECT(int_pointer_t, p_null_expression);
OBJECT(handler_t, h_null);
OBJECT(long, where);
OBJECT(unsigned long, where_element);
OBJECT(char, c_after);

#define ARRAY_OBJECT(T, name, size) extern T name[size], ref_##name[size]

ARRAY_OBJECT(int, a, ARRAY);
ARRAY_OBJECT(int, early, 5);
ARRAY_OBJECT(int, tentative, 1);
ARRAY_OBJECT(int, sized_later, 6);
ARRAY_OBJECT(int, reversed, 3);
ARRAY_OBJECT(int, probe_array, PROBE_ARRAY);
ARRAY_OBJECT(char, c_whole, 4);
ARRAY_OBJECT(char, c_padded, 8);
ARRAY_OBJECT(char, c_full, 3);
ARRAY_OBJECT(char, c_braced, 4);
ARRAY_OBJECT(signed char, c_signed, 4);
ARRAY_OBJECT(unsigned char, c_unsigned, 5);
ARRAY_OBJECT(char, c_escaped, sizeof ESCAPED);
ARRAY_OBJECT(char, c_empty, 4);
ARRAY_OBJECT(char, c_long, sizeof LONG_WORD);

extern int m[ROWS][COLUMNS], ref_m[ROWS][COLUMNS];
extern int ref_lib_object, ref_lib_array[ARRAY];
extern int lib_array[ARRAY];

/* declarations: each function and its reference */
#define BOTH(R, f, parameters) \
        R f parameters;        \
        R ref_##f parameters;

BOTH(int, twice, (int))
BOTH(void, on_signal, (int))
BOTH(int, lib_triple, (int))
BOTH(long, lib_total, (void))
BOTH(int, read_early, (int))
BOTH(long, size_early, (void))
BOTH(long, size_sized_later, (void))
BOTH(long, size_reversed, (void))
BOTH(long, size_probe_array, (void))
BOTH(long, size_whole, (void))
BOTH(long, size_braced, (void))
BOTH(long, size_signed, (void))
BOTH(long, size_escaped, (void))
BOTH(long, size_long, (void))
BOTH(void, set_tentative, (int))
BOTH(int, is_n, (void))
BOTH(int, is_element, (void))
BOTH(int, is_cell, (void))
BOTH(int, is_row, (void))
BOTH(int, is_late, (void))
BOTH(int, is_object, (void))
BOTH(int, is_array, (void))
BOTH(int, is_pointer, (void))
BOTH(int, is_signal, (void))
BOTH(int, is_twice, (void))
BOTH(int, is_triple, (void))
BOTH(int, is_where, (void))
BOTH(long, through_element, (void))
BOTH(long, through_difference, (void))
BOTH(long, through_row, (int))
BOTH(long, call_twice, (int))
BOTH(long, call_triple, (int))
BOTH(void, call_signal, (int))
BOTH(long, read_word, (int))
BOTH(long, read_suffix, (int))
BOTH(long, read_lib_element, (void))
BOTH(long, lib_element_index, (void))
BOTH(long, read_lib_suffix, (int))
BOTH(long, read_lib_word, (int))
BOTH(long, call_lib_function, (int))
BOTH(int, is_lib_function, (void))

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

/* an object's bytes in both builds against the bytes it should hold */
#define BYTES(name, want)                                 \
        PAIR(#name, memcmp(name, want, sizeof name) == 0, \
             memcmp(ref_##name, want, sizeof ref_##name) == 0, 1)

/* The addresses the initializers wrote, against the addresses of each
   build's own objects, before anything writes them. */
static void
test_addresses(void)
{
        PAIR("p_n", p_n == &n, ref_p_n == &ref_n, 1);
        PAIR("p_element", p_element - a, ref_p_element - ref_a, 3);
        PAIR("p_decay", p_decay == a, ref_p_decay == ref_a, 1);
        PAIR("p_sum", p_sum - a, ref_p_sum - ref_a, 2);
        PAIR("p_difference", p_difference - a, ref_p_difference - ref_a, 3);
        PAIR("p_cell", p_cell == &m[1][2], ref_p_cell == &ref_m[1][2], 1);
        PAIR("p_row", p_row == &m[1], ref_p_row == &ref_m[1], 1);
        PAIR("p_row_decay", p_row_decay == m[2], ref_p_row_decay == ref_m[2],
             1);
        PAIR("p_bytes", p_bytes - (char *) &n, ref_p_bytes - (char *) &ref_n,
             1);
        PAIR("p_cast", p_cast == (long *) &m[2],
             ref_p_cast == (long *) &ref_m[2], 1);
        PAIR("p_void", p_void == &n, ref_p_void == &ref_n, 1);
        PAIR("p_pointer", p_pointer == &p_n, ref_p_pointer == &ref_p_n, 1);
        PAIR("p_late", p_late == &late, ref_p_late == &ref_late, 1);
        PAIR("p_object", p_object == &lib_object,
             ref_p_object == &ref_lib_object, 1);
        PAIR("p_array", p_array - lib_array, ref_p_array - ref_lib_array, 5);
        PAIR("h_signal", h_signal == on_signal, ref_h_signal == ref_on_signal,
             1);
        PAIR("h_signal_address", h_signal_address == on_signal,
             ref_h_signal_address == ref_on_signal, 1);
        PAIR("f_twice", f_twice == twice, ref_f_twice == ref_twice, 1);
        PAIR("f_triple", f_triple == lib_triple, ref_f_triple == ref_lib_triple,
             1);
        PAIR("h_cast", h_cast == (thunk_t) twice,
             ref_h_cast == (thunk_t) ref_twice, 1);
        PAIR("p_fixed", (long) p_fixed, (long) ref_p_fixed, 4096);
        PAIR("p_fixed_sum", (long) p_fixed_sum, (long) ref_p_fixed_sum,
             4096 + 3 * sizeof(int));
        PAIR("p_null", p_null == 0, ref_p_null == 0, 1);
        PAIR("p_void_null", p_void_null == 0, ref_p_void_null == 0, 1);
        PAIR("p_null_expression", p_null_expression == 0,
             ref_p_null_expression == 0, 1);
        PAIR("h_null", h_null == 0, ref_h_null == 0, 1);
        PAIR("where", where == (long) &n, ref_where == (long) &ref_n, 1);
        PAIR("where_element", where_element == (unsigned long) &a[2],
             ref_where_element == (unsigned long) &ref_a[2], 1);
}

/* The strings the pointers point into, and the arrays of char the
   strings initialize, byte by byte, the zeros past the string
   included; and the sizes each build's code gives the arrays. */
static void
test_strings(void)
{
        PAIR("s_word", strcmp(s_word, WORD), strcmp(ref_s_word, WORD), 0);
        PAIR("s_suffix", strcmp(s_suffix, "bc"), strcmp(ref_s_suffix, "bc"), 0);
        PAIR("s_element", strcmp(s_element, "z"), strcmp(ref_s_element, "z"),
             0);
        PAIR("s_escaped", strcmp(s_escaped, ESCAPED),
             strcmp(ref_s_escaped, ESCAPED), 0);

        BYTES(c_whole, "abc");
        BYTES(c_padded, "abc\0\0\0\0");
        BYTES(c_full, "abc");
        BYTES(c_braced, "abc");
        BYTES(c_signed, "abc");
        BYTES(c_unsigned, "\377\200\0\0");
        BYTES(c_escaped, ESCAPED);
        BYTES(c_empty, "\0\0\0");
        BYTES(c_long, LONG_WORD);
        PAIR("c_after", c_after, ref_c_after, 'z');

        PAIR("size_whole", size_whole(), ref_size_whole(), 4);
        PAIR("size_braced", size_braced(), ref_size_braced(), 4);
        PAIR("size_signed", size_signed(), ref_size_signed(), 4);
        PAIR("size_escaped", size_escaped(), ref_size_escaped(),
             sizeof ESCAPED);
        PAIR("size_long", size_long(), ref_size_long(), sizeof LONG_WORD);

        /* rcc writes the arrays in the order of their definitions, each
           of alignment 1, so one array's end is the next one's label:
           an array of the wrong size moves it */
        report("c_full end", c_braced - c_full, 3);
        report("c_whole end", c_padded - c_whole, 4);

#if defined(__x86_64__)
        /* the psABI aligns an array of 16 bytes or more to 16 */
        report("c_long alignment", (long) ((unsigned long) c_long % 16), 0);
#endif
}

/* The arrays declared without their size: the size a later
   declaration gives, the one element of a tentative definition, and an
   array the other file defines or declares. */
static void
test_sizes(void)
{
        PAIR("size_early", size_early(), ref_size_early(), 5 * sizeof(int));
        PAIR("size_sized_later", size_sized_later(), ref_size_sized_later(),
             6 * sizeof(int));
        PAIR("size_reversed", size_reversed(), ref_size_reversed(),
             3 * sizeof(int));
        PAIR("size_probe_array", size_probe_array(), ref_size_probe_array(),
             PROBE_ARRAY * sizeof(int));
        PAIR("tentative", tentative[0], ref_tentative[0], 0);

        for (int i = 0; i < 5; ++i) {
                early[i] = ref_early[i] = 100 + i;
                PAIR("read_early", read_early(i), ref_read_early(i), 100 + i);
        }

        after_tentative = ref_after_tentative = 77;
        set_tentative(9);
        ref_set_tentative(9);
        PAIR("set_tentative", tentative[0], ref_tentative[0], 9);
        PAIR("after_tentative", after_tentative, ref_after_tentative, 77);

        for (int i = 0; i < PROBE_ARRAY; ++i)
                probe_array[i] = ref_probe_array[i] = 3 + i;
        PAIR("lib_total", lib_total(), ref_lib_total(), 3 + 8 + 15 + 24);
}

/* What the addresses designate, read by the main and by each build's
   code, and the comparisons each build's code makes of the addresses
   in its data with those it forms. */
static void
test_designated(void)
{
        for (int i = 0; i < ARRAY; ++i)
                a[i] = ref_a[i] = 11 * i;

        for (int i = 0; i < ROWS; ++i)
                for (int j = 0; j < COLUMNS; ++j)
                        m[i][j] = ref_m[i][j] = 10 * i + j;

        PAIR("*p_element", *p_element, *ref_p_element, 33);
        PAIR("*p_cell", *p_cell, *ref_p_cell, 12);
        PAIR("through_element", through_element(), ref_through_element(), 33);
        PAIR("through_difference", through_difference(),
             ref_through_difference(), 3);

        for (int j = 0; j < COLUMNS; ++j)
                PAIR("through_row", through_row(j), ref_through_row(j), 10 + j);

        PAIR("is_n", is_n(), ref_is_n(), 1);
        PAIR("is_element", is_element(), ref_is_element(), 1);
        PAIR("is_cell", is_cell(), ref_is_cell(), 1);
        PAIR("is_row", is_row(), ref_is_row(), 1);
        PAIR("is_late", is_late(), ref_is_late(), 1);
        PAIR("is_object", is_object(), ref_is_object(), 1);
        PAIR("is_array", is_array(), ref_is_array(), 1);
        PAIR("is_pointer", is_pointer(), ref_is_pointer(), 1);
        PAIR("is_signal", is_signal(), ref_is_signal(), 1);
        PAIR("is_twice", is_twice(), ref_is_twice(), 1);
        PAIR("is_triple", is_triple(), ref_is_triple(), 1);
        PAIR("is_where", is_where(), ref_is_where(), 1);
        PAIR("*p_late", *p_late, *ref_p_late, 5);
        PAIR("*p_object", *p_object, *ref_p_object, 31);
        PAIR("*p_array", *p_array, *ref_p_array, 15);

        PAIR("call_twice", call_twice(21), ref_call_twice(21), 42);
        PAIR("call_triple", call_triple(-7), ref_call_triple(-7), -21);
        PAIR("f_twice", f_twice(8), ref_f_twice(8), 16);

        call_signal(5);
        ref_call_signal(5);
        PAIR("call_signal", signalled, ref_signalled, 5);
        h_signal_address(7);
        ref_h_signal_address(7);
        PAIR("h_signal_address", signalled, ref_signalled, 7);

        for (int i = 0; i < 4; ++i) {
                PAIR("read_word", read_word(i), ref_read_word(i), WORD[i]);
                PAIR("read_suffix", read_suffix(i % 3), ref_read_suffix(i % 3),
                     WORD[1 + i % 3]);
        }
}

/* The objects initializers-lib.c initializes, read by rcc's code and
   by the reference's. */
static void
test_library(void)
{
        PAIR("read_lib_element", read_lib_element(), ref_read_lib_element(),
             16);
        PAIR("lib_element_index", lib_element_index(), ref_lib_element_index(),
             6);
        PAIR("read_lib_suffix", read_lib_suffix(0), ref_read_lib_suffix(0),
             'c');
        PAIR("read_lib_suffix", read_lib_suffix(1), ref_read_lib_suffix(1), 0);

        for (int i = 0; i < 4; ++i)
                PAIR("read_lib_word", read_lib_word(i), ref_read_lib_word(i),
                     WORD[i]);

        PAIR("call_lib_function", call_lib_function(5),
             ref_call_lib_function(5), 15);
        PAIR("is_lib_function", is_lib_function(), ref_is_lib_function(), 1);
}

int
main(void)
{
        if (getenv("PROBE_ALL"))
                shown = LONG_MAX;

        test_addresses();
        test_strings();
        test_sizes();
        test_designated();
        test_library();

        printf("%ld checks, %ld differ\n", total, bad);

        return bad != 0;
}
