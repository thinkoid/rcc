/* A program with stdlib.h, the system's, written in GNU C, which rcc
   reads as it is: among its declarations the parameter of atexit, a
   pointer to a function, and the typedef of qsort's comparison, which
   the program does not use.  The program converts three numbers with
   atoi, abs and strtol and checks each, so its exit status says that
   the declarations the header gave it are the functions it called.

   Under rcc and under clang -O2 it exits 0: nothing here is undefined,
   and the row is the same on both sides.

   On x86-64 the listing must show the calls by the names the header
   declares, call atoi@PLT, call abs@PLT and call strtol@PLT, and
   nothing for any declaration of the header.

   On aarch64 the listing must show bl atoi, bl abs and bl strtol, and
   nothing for any declaration of the header. */
#include <stdlib.h>

int
main(void)
{
        if (atoi("42") != 42)
                return 1;

        if (abs(-7) != 7)
                return 2;

        if (strtol("-ff", 0, 16) != -255)
                return 3;

        return 0;
}
