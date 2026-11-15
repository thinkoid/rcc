/* The braces probe's library, compiled by clang alone and linked
   twice, once as it is for rcc's build and once renamed under the
   prefix ref_ for the reference's, so each build has its own copy.  It
   defines the function a table of braces.c points to, and objects
   clang initializes by lists, braced in full and in part, of pointers
   to strings and of arrays of char, which braces.c reads.  See
   braces.h. */

#include "braces.h"

int lib_table[ROWS][COLUMNS] = { { 1, 2, 3 }, { 4, 5, 6 } };
int lib_elided[ROWS][COLUMNS] = { { 7 }, 8, 9 };
char *lib_names[] = { "Apr", "May" };
char lib_words[][4] = { "Jun", { 'J', 'u', 'l' } };

int
lib_triple(int v)
{
        return 3 * v;
}
