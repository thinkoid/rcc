/* A program in the manner of K&R's, which its globals' initializers
   make possible: a table of names, a pointer to a function and a
   pointer to the library's stream, each initialized at file scope.
   months is the table, the names of the twelve months three letters
   each, an array of char whose size the string gives, 37 bytes; title
   points to a string literal; out holds the address of stdout, the
   library's object, put the address of putchar, the library's
   function, and print that of fputs.  The program writes the line
   "months: Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec" with
   print and put, through out, its newline a literal main names, then
   checks that out points to stdout and that put is putchar, as the
   loader filled them in.  Its exit status, sizeof months, says that
   the array took the string's size.

   Under rcc and under clang -O2 it prints the line and exits 37:
   nothing here is undefined, and the row is the same on both sides.

   On x86-64 and on aarch64 the data must show the literal of title in
   the literals' block after the one main names, .Ls0 as .string "\n"
   and .Ls1 as .string "months:"; months as .size months, 37 and its
   own bytes, .string "JanFeb...Dec", under .p2align 4 on x86-64 and
   none on aarch64; title as .quad .Ls1, out as .quad stdout, put as
   .quad putchar and print as .quad fputs, each in .data. */
#include <stdio.h>

char months[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
char *title = "months:";
FILE **out = &stdout;
int (*put)(int) = putchar;
int (*print)(const char *, FILE *) = fputs;

/* the name of month n, 1 to 12: three letters of the table, without a
   terminating zero */
char *
month(int n)
{
        return months + 3 * (n - 1);
}

int
main(void)
{
        int n, i;
        char *name;

        if (print(title, *out) < 0)
                return 1;

        for (n = 1; n <= 12; n++) {
                put(' ');
                name = month(n);
                for (i = 0; i < 3; i++)
                        put(name[i]);
        }

        if (print("\n", *out) < 0)
                return 2;

        if (*out != stdout)
                return 3;

        if (put != putchar)
                return 4;

        return sizeof months;
}
