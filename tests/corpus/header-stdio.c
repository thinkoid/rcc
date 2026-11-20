/* A program with its headers.  stdio.h and string.h are the system's,
   written in GNU C, and rcc reads them as they are: typedefs, structs
   it declares and does not open, attributes, restrict.  The program
   formats a line with snprintf, checks its length and its bytes, and
   prints it, so its exit status says that the declarations the headers
   gave it are the functions it called.

   Under rcc and under clang -O2 it prints "hello, world 42" and
   exits 0: nothing here is undefined, and the row is the same on both
   sides.

   On x86-64 the listing must show the calls by the names the headers
   declare, call snprintf@PLT, call strcmp@PLT and call printf@PLT, and
   nothing for any declaration of the headers.

   On aarch64 the listing must show bl snprintf, bl strcmp and bl
   printf, and nothing for any declaration of the headers. */
#include <stdio.h>
#include <string.h>

int
main(void)
{
        char line[32];
        int n;

        n = snprintf(line, sizeof line, "hello, %s %d", "world", 42);
        if (n != 15)
                return 1;

        if (strcmp(line, "hello, world 42") != 0)
                return 2;

        printf("%s\n", line);
        return 0;
}
