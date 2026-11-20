/* A program with its own globals and the C library's.  count, table
   and total are the program's, defined here; stdout is the library's,
   declared by stdio.h as extern FILE *stdout and defined in the shared
   library.  The program fills the table with the squares of 0 to 9,
   counting them in count, sums the table into total, writes the line
   "10 squares, total 285" to stdout with fprintf and checks its length,
   then writes "done" with fputs.  Its exit status, table[3] + count,
   says that each object held what was written to it, and the checks
   that stdout was the library's stream.

   Under rcc and under clang -O2 it prints the two lines and exits 19:
   nothing here is undefined, and the row is the same on both sides.

   On x86-64 the listing must show the program's objects reached
   PC-relative, addl $1, count(%rip), movl count(%rip) and
   leaq table(%rip); stdout's address loaded from the GOT,
   movq stdout@GOTPCREL(%rip), and stdout read through it,
   movq (%rax), %rax; count and table in .bss and total in .data.

   On aarch64 the listing must show each access of count through
   adrp x16, count and [x16, :lo12:count], table's elements through
   its page, adrp x0, table, and [x0, :lo12:table], and stdout's address
   loaded from the GOT, adrp x0, :got:stdout and
   ldr x0, [x0, :got_lo12:stdout], then stdout read through it,
   ldr x0, [x0]; the data as on x86-64. */
#include <stdio.h>

int count;
int table[10];
long total = 0;

int
main(void)
{
        int i;

        for (i = 0; i < 10; i++) {
                table[i] = i * i;
                count++;
        }

        for (i = 0; i < count; i++)
                total += table[i];

        if (total != 285)
                return 1;

        if (fprintf(stdout, "%d squares, total %ld\n", count, total) != 22)
                return 2;

        if (fputs("done\n", stdout) < 0)
                return 3;

        return table[3] + count;
}
