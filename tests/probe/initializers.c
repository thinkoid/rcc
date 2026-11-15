/* The initializers probe: objects at file scope initialized by an
   address constant or a string, in rcc's C.  Each spelling of an
   address constant: the address of an object this file defines, of an
   element, of a row, of an object or a function it only declares, an
   array or a function that decays, a sum and a difference, a pointer
   cast, a string literal and a pointer into one, an integer cast to a
   pointer, a null pointer, an address cast to an integer of 8 bytes.
   Each shape of an array of char a string initializes.  Arrays
   declared without their size, completed by a later declaration, by
   the other file or by the end of this one.  The functions read what
   the objects point to and compare each address with the one the code
   forms; others read the objects initializers-lib.c initializes.  See
   initializers.h. */

#include "initializers.h"

/* the objects the addresses designate, and the functions */
int n;
int a[ARRAY];
int m[ROWS][COLUMNS];
int signalled;

int twice(int v);
void on_signal(int v);
extern int late;

/* address constants, 6.6p9, p11 */
int *p_n = &n;
int *p_element = &a[3];
int *p_decay = a;
int *p_sum = a + 2;
int *p_difference = &a[5] - 2;
int *p_cell = &m[1][2];
int (*p_row)[COLUMNS] = &m[1];
int *p_row_decay = m[2];
char *p_bytes = (char *) &n + 1;
long *p_cast = (long *) &m[2];
void *p_void = &n;
int **p_pointer = &p_n;
int *p_late = &late;
int *p_object = &lib_object;
int *p_array = lib_array + 5;
void (*h_signal)(int) = on_signal;
void (*h_signal_address)(int) = &on_signal;
int (*f_twice)(int) = twice;
int (*f_triple)(int) = lib_triple;
void (*h_cast)(void) = (void (*)(void)) twice;
char *s_word = WORD;
char *s_suffix = WORD + 1;
char *s_element = &"xyz"[2];
char *s_escaped = ESCAPED;
int *p_fixed = (int *) 4096;
int *p_fixed_sum = (int *) 4096 + 3;
int *p_null = 0;
int *p_void_null = (void *) 0;
int *p_null_expression = 1 - 1;
void (*h_null)(int) = 0;
long where = (long) &n;
unsigned long where_element = (unsigned long) &a[2];

/* defined after the use of its address */
int late = 5;

/* arrays of char a string initializes, 6.7.11p15, each followed by an
   object whose first byte is not zero, so a short array shows in the
   bytes read past it */
char c_whole[] = WORD;
char c_padded[8] = WORD;
char c_full[3] = WORD;
char c_braced[] = { WORD };
signed char c_signed[] = WORD;
unsigned char c_unsigned[5] = "\377\200";
char c_escaped[] = ESCAPED;
char c_empty[4] = "";
char c_long[] = LONG_WORD;
char c_after = 'z';

/* arrays without their size: early is used before a later declaration
   gives its size; tentative is one element at the end of the file,
   6.9.3p2, before after_tentative; sized_later is tentative twice, the
   second time with its size; reversed has its size first; probe_array
   is declared without its size in initializers.h */
extern int early[];

int
read_early(int i)
{
        return early[i];
}

int early[5];
int tentative[];
int after_tentative;
int sized_later[];
int sized_later[6];
int reversed[3];
extern int reversed[];
int probe_array[PROBE_ARRAY];

int
twice(int v)
{
        return v + v;
}

void
on_signal(int v)
{
        signalled = v;
}

/* the sizes the declarations give the arrays */
long
size_early(void)
{
        return sizeof early;
}

long
size_sized_later(void)
{
        return sizeof sized_later;
}

long
size_reversed(void)
{
        return sizeof reversed;
}

long
size_probe_array(void)
{
        return sizeof probe_array;
}

long
size_whole(void)
{
        return sizeof c_whole;
}

long
size_braced(void)
{
        return sizeof c_braced;
}

long
size_signed(void)
{
        return sizeof c_signed;
}

long
size_escaped(void)
{
        return sizeof c_escaped;
}

long
size_long(void)
{
        return sizeof c_long;
}

/* tentative written by the code, which must leave after_tentative */
void
set_tentative(int v)
{
        tentative[0] = v;
}

/* each address in the data against the address the code forms */
int
is_n(void)
{
        return p_n == &n;
}

int
is_element(void)
{
        return p_element == &a[3];
}

int
is_cell(void)
{
        return p_cell == &m[1][2];
}

int
is_row(void)
{
        return p_row == &m[1];
}

int
is_late(void)
{
        return p_late == &late;
}

int
is_object(void)
{
        return p_object == &lib_object;
}

int
is_array(void)
{
        return p_array == &lib_array[5];
}

int
is_pointer(void)
{
        return p_pointer == &p_n;
}

int
is_signal(void)
{
        return h_signal == on_signal && h_signal_address == &on_signal;
}

int
is_twice(void)
{
        return f_twice == twice;
}

int
is_triple(void)
{
        return f_triple == lib_triple;
}

int
is_where(void)
{
        return where == (long) &n;
}

/* what the addresses designate, through the code */
long
through_element(void)
{
        return *p_element;
}

long
through_difference(void)
{
        return p_difference - a;
}

long
through_row(int j)
{
        return (*p_row)[j];
}

long
call_twice(int v)
{
        return f_twice(v);
}

long
call_triple(int v)
{
        return (*f_triple)(v);
}

void
call_signal(int v)
{
        h_signal(v);
}

long
read_word(int i)
{
        return s_word[i];
}

long
read_suffix(int i)
{
        return s_suffix[i];
}

/* the objects initializers-lib.c initializes, read by rcc's code */
long
read_lib_element(void)
{
        return *lib_element;
}

long
lib_element_index(void)
{
        return lib_element - lib_array;
}

long
read_lib_suffix(int i)
{
        return lib_suffix[i];
}

long
read_lib_word(int i)
{
        return lib_word[i];
}

long
call_lib_function(int v)
{
        return lib_function(v);
}

int
is_lib_function(void)
{
        return lib_function == lib_triple;
}
