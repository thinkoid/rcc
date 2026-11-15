/* K&R's count of digits, white space and other characters, section 3.4
   of the second edition, written from the book's description, which
   switch makes possible.  The program counts each digit, the blanks,
   newlines and tabs together, and every other character, with a switch
   on the character: the ten digits' cases share one statement, the
   three of white space another, and the default counts the rest.  It
   reads a string, not its standard input, so that it runs alone.

   The switch has thirteen cases, '\t', '\n', ' ' and '0' to '9', 9 to
   57: ncase 13 and range 48, above 3 * 13 = 39, so the form is the hash
   (backend.md 7.3).  Of the sizes 3 to 6, i * worst is 15 for 3, 16 for
   4, 15 for 5 and 18 for 6, so the size is 3, the first of the least.

   The program counts "In 1978 K&R had 228 pages;\n\tthe 2nd edition,
   1988, has 272.\n" and checks each count with ==.  It writes the line
   "digits = 0 2 5 0 0 0 0 2 4 2, white space = 13, other = 32" and
   exits 32, the count of the other characters, or exits 100 plus the
   number of the first check that failed.

   Under rcc and under clang -O2 it prints the line and exits 32:
   nothing here is undefined.

   On x86-64 and on aarch64 the listing must show the hash's division by
   3, divl %ecx after movl $3, %ecx on x86-64 and udiv after mov w1, #3
   on aarch64; the jump through a table of three buckets by the
   remainder; and each bucket's compares of the quotient, the case of
   '0', 48, as cmpl $16, %eax and cmp w2, #16. */
#include <stdio.h>

/* the text the program counts */
static char text[] =
    "In 1978 K&R had 228 pages;\n\tthe 2nd edition, 1988, has 272.\n";

int
main(void)
{
        int c, i, nwhite, nother;
        int ndigit[10];

        nwhite = nother = 0;
        for (i = 0; i < 10; i++)
                ndigit[i] = 0;

        for (i = 0; (c = text[i]) != '\0'; i++) {
                switch (c) {
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9': ndigit[c - '0']++; break;
                case ' ':
                case '\n':
                case '\t': nwhite++; break;
                default: nother++; break;
                }
        }

        printf("digits =");
        for (i = 0; i < 10; i++)
                printf(" %d", ndigit[i]);
        printf(", white space = %d, other = %d\n", nwhite, nother);

        if (ndigit[1] != 2 || ndigit[2] != 5 || ndigit[7] != 2)
                return 101;
        if (ndigit[8] != 4 || ndigit[9] != 2 || ndigit[0] != 0)
                return 102;
        if (nwhite != 13)
                return 103;

        return nother;
}
