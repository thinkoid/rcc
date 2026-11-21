/* The braces probe: objects at file scope initialized by
   brace-enclosed lists, in rcc's C.  Arrays of integers of each width,
   of pointers and of char, one-dimensional and of arrays, to three
   levels: the size from the count, the elements past the list zero, a
   trailing comma, the empty initializer; the braces of a subaggregate
   written, left out and left out in part, 6.7.11p21, among them
   examples 3, 4 and 6 of 6.7.11; an element of pointer type an address
   constant of each spelling; an element that is an array of char
   initialized by a string; a scalar in braces.  Each array is followed
   by a guard of the alignment of its element, so that an array written
   short or long shows in the guard.  The functions read the objects
   through rcc's code and give the sizes the lists complete; others
   read the objects braces-lib.c initializes.  See braces.h. */

#include "braces.h"

/* the objects and the functions the addresses designate */
int n = 7;
int target[4] = { 10, 11, 12, 13 };

int twice(int v);

/* one dimension: the size from the count, p23; the rest zero, p22; a
   trailing comma, p1; the empty initializer, p11; 32 bytes, which
   x86-64 aligns to 16 */
int counted[] = { 1, 2, 3 };
int g_counted = GUARD;
int padded[5] = { 1, 2 };
int g_padded = GUARD;
int trailing[3] = {
        4,
        5,
        6,
};
int g_trailing = GUARD;
int trailing_counted[] = {
        7,
        8,
};
int g_trailing_counted = GUARD;
int empty[3] = { };
int g_empty = GUARD;
int big[8] = { 1 };
int g_big = GUARD;

/* each width, each element converted to its type as by assignment,
   p12 */
signed char narrow[] = { -1, 127, -128, 300 };
char g_narrow = GUARD;
unsigned char bytes[] = { 255, 0x80, -1 };
char g_bytes = GUARD;
short shorts[3] = { -3, 40000 };
short g_shorts = GUARD;
unsigned short ushorts[] = { 65535, -2 };
short g_ushorts = GUARD;
long longs[] = { -1, 1L << 40, 'a' };
long g_longs = GUARD;
unsigned long ulongs[2] = { -1 };
long g_ulongs = GUARD;

/* arrays of arrays: braced in full, flat, braced in part, the outer
   size from a flat list; examples 3, 4 and 6 of 6.7.11, the last in
   its three spellings; a scalar in its own braces, an empty row, and
   a row of rows braced while its rows are not */
int full[ROWS][COLUMNS] = { { 1, 2, 3 }, { 4, 5, 6 } };
int g_full = GUARD;
int flat[ROWS][COLUMNS] = { 1, 2, 3, 4, 5, 6 };
int g_flat = GUARD;
int partly[ROWS][COLUMNS] = { { 1 }, 4, 5 };
int g_partly = GUARD;
int rows_counted[][COLUMNS] = { 1, 2, 3, 4 };
int g_rows_counted = GUARD;
int y[4][3] = {
        { 1, 3, 5 },
        { 2, 4, 6 },
        { 3, 5, 7 },
};
int g_y = GUARD;
int z[4][3] = { { 1 }, { 2 }, { 3 }, { 4 } };
int g_z = GUARD;
short q[4][3][2] = { { 1 }, { 2, 3 }, { 4, 5, 6 } };
short g_q = GUARD;
short q_flat[4][3][2] = { 1, 0, 0, 0, 0, 0, 2, 3, 0, 0, 0, 0, 4, 5, 6 };
short g_q_flat = GUARD;
short q_full[4][3][2] = { {
                              { 1 },
                          },
                          {
                              { 2, 3 },
                          },
                          {
                              { 4, 5 },
                              { 6 },
                          } };
short g_q_full = GUARD;
int scalar_braces[ROWS][COLUMNS] = {
        { { 1 }, 2 },
        { },
};
int g_scalar_braces = GUARD;
int deep[2][2][2] = { { 1, 2, 3 }, 4 };
int g_deep = GUARD;

/* pointers: an address constant each, 6.6p9, a string literal's, an
   object's, an element's, a row's, a function's, a null pointer and
   an integer cast to a pointer */
char *names[] = { "Jan", "Feb", "Mar" };
long g_names = GUARD;
int *pointers[] = { &n, target + 2, &target[3], 0, (void *) 0, (int *) 4096 };
long g_pointers = GUARD;
unary_t functions[3] = { twice, lib_triple };
long g_functions = GUARD;
int *rows[] = { full[1], *flat };
long g_rows = GUARD;
row_pointer_t row_pointers[] = { &full[1], partly, partly + 1 };
long g_row_pointers = GUARD;
char *suffixes[2][2] = { { "ab" + 1 }, "cd", "ef" };
long g_suffixes = GUARD;

/* arrays of char: a string initializes an element that is an array of
   char by the element's own bytes, p15, braced or not, with its
   terminating zero when there is room; the elements spelled one by
   one */
char months[][4] = { "Jan", "Feb", "Mar" };
char g_months = GUARD;
char letters[ROWS][4] = { { 'J', 'a', 'n' }, "Feb" };
char g_letters = GUARD;
char exact[2][3] = { "abc", "de" };
char g_exact = GUARD;
char padded_words[3][6] = { "ab" };
char g_padded_words = GUARD;
char words[2][2][4] = { { "ab", "cd" }, { "ef" } };
char g_words = GUARD;
char spelled[2][3] = { 'a', 'b', 'c', 'd' };
char g_spelled = GUARD;
char braced_string[] = { "abc" };
char g_braced_string = GUARD;
char braced_words[][3] = {
        { "ab" },
        { "c" },
};
char g_braced_words = GUARD;

/* scalars in braces, p12, and the empty initializer, p11 */
int scalar = { 5 };
int scalar_comma = {
        6,
};
int scalar_empty = { };
long scalar_long = { -2 };
char *string_pointer = { "abc" };
int *pointer_empty = { };

/* designators, p18 to p20: the walk moved to the subobject a
   designation names, going on from the next one; a later value of a
   subobject over an earlier one, a braced list's over every earlier
   value in its subobject; another member of a union over the whole
   union; an element over its part of a string; the size of an array
   of unknown size from its highest index, p23; a member of an
   anonymous member, 6.7.3.2p15, the walk entering the anonymous
   member and going on in it */
int designated[6] = { [4] = 5, [1] = 2, 3 };
int g_designated = GUARD;
int designated_counted[] = { [9] = 1, [2] = 7, 8 };
int g_designated_counted = GUARD;
int designated_again[4] = { 1, 2, 3, 4, [1] = 9, [0] = 8 };
int g_designated_again = GUARD;
int designated_rows[3][COLUMNS] = { [1][1] = 5, 6, [0] = { 7 },
                                    [2][0] = 8, 9, [1] = { [2] = 4 } };
int g_designated_rows = GUARD;
short designated_deep[2][2][2] = { [1][0] = { 1, 2 }, [0][1][1] = 3, 4 };
short g_designated_deep = GUARD;
char designated_words[3][4] = {
        [2] = "ab", [0][1] = 'c', 'd', [1] = "ef", [2][1] = 'x'
};
char g_designated_words = GUARD;
char *designated_names[4] = { [3] = "Dec", [1] = "Feb", [3] = "Mar" };
long g_designated_names = GUARD;
struct designated_line designated_lines[3] = {
        [1] = { .to = { 7 } },
        [0].to.y = 8,
        [1].from.x = 9,
        10,
        [2] = { .to.x = 3, 4, .from.y = 2, .from = { 1 } },
};
int g_designated_lines = GUARD;
union designated_number designated_numbers[3] = { [0].i = -1,
                                                  [0].c = 'x',
                                                  [1] = { .c = 'y' },
                                                  [2].c = 'z',
                                                  [2].i = 0x01020304 };
int g_designated_numbers = GUARD;
struct designated_named designated_named[] = {
        [0] = { .name = "abc", .name[1] = 'X', 'Y', .n = 5 },
        [1].name = "pq",
        [1].name[2] = 'r',
};
int g_designated_named = GUARD;
struct designated_variant designated_variants[3] = {
        [0] = { .i = 1, 2 },
        [1] = { .i = -1, .c = 'x', 3 },
        [2].c = 'y',
        4,
};
int g_designated_variants = GUARD;
struct designated_pair designated_pairs[2] = {
        [0] = { .a = 1, 2, 3 },
        [1] = { .b = 4, 5, .tag = 6 },
};
int g_designated_pairs = GUARD;
struct designated_nested designated_nested[3] = {
        [0] = { .y = 1, 2 },
        [1] = { .x = 3, .whole = 4, 5 },
        [2] = { .whole = 6, .y = 7 },
};
int g_designated_nested = GUARD;

int
twice(int v)
{
        return v + v;
}

/* the sizes the lists complete */
long
size_counted(void)
{
        return sizeof counted;
}

long
size_trailing_counted(void)
{
        return sizeof trailing_counted;
}

long
size_narrow(void)
{
        return sizeof narrow;
}

long
size_rows_counted(void)
{
        return sizeof rows_counted;
}

long
size_names(void)
{
        return sizeof names;
}

long
size_pointers(void)
{
        return sizeof pointers;
}

long
size_months(void)
{
        return sizeof months;
}

long
size_braced_string(void)
{
        return sizeof braced_string;
}

long
size_braced_words(void)
{
        return sizeof braced_words;
}

long
size_designated_counted(void)
{
        return sizeof designated_counted;
}

long
size_designated_named(void)
{
        return sizeof designated_named;
}

/* the objects read through the code */
long
read_partly(int i, int j)
{
        return partly[i][j];
}

long
read_q(int i, int j, int k)
{
        return q[i][j][k];
}

long
read_ulongs(int i)
{
        return ulongs[i] == (unsigned long) -1;
}

long
read_names(int i, int k)
{
        return names[i][k];
}

long
read_months(int i, int j)
{
        return months[i][j];
}

long
read_suffixes(int i, int j, int k)
{
        return suffixes[i][j][k];
}

long
call_function(int i, int v)
{
        return functions[i](v);
}

long
through_pointer(int i)
{
        return *pointers[i];
}

long
through_row(int i, int j)
{
        return rows[i][j];
}

long
through_row_pointer(int i, int j)
{
        return (*row_pointers[i])[j];
}

/* each address in the data against the address the code forms */
int
is_pointers(void)
{
        return pointers[0] == &n && pointers[1] == &target[2] &&
               pointers[2] == target + 3 && !pointers[3] && !pointers[4];
}

int
is_functions(void)
{
        return functions[0] == twice && functions[1] == lib_triple &&
               !functions[2];
}

int
is_rows(void)
{
        return rows[0] == &full[1][0] && rows[1] == &flat[0][0] &&
               row_pointers[0] == full + 1 && row_pointers[2] == &partly[1];
}

/* the objects braces-lib.c initializes, read by rcc's code */
long
read_lib_table(int i, int j)
{
        return lib_table[i][j];
}

long
read_lib_elided(int i, int j)
{
        return lib_elided[i][j];
}

long
read_lib_names(int i, int k)
{
        return lib_names[i][k];
}

long
read_lib_words(int i, int j)
{
        return lib_words[i][j];
}
