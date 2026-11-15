/* K&R's getch and ungetch, section 4.3 of the second edition, with the
   static buffer of section 4.6, written from the book's description:
   getch() takes the character ungetch() pushed back last, or else the
   next of the input, and ungetch() pushes one back.  buf and bufp are
   static, so no other file sees them.  The input is a string here, not
   standard input, so the program does not depend on what it is given:
   read_char() takes its next character, through a static pointer, or
   EOF at its end.  getint() reads an integer in the manner of section
   5.2: blanks skipped, a sign, the digits, and the character after them
   pushed back.  The program sums the integers of " 12 -7  +30\t100\n",
   pushes "abc" back and reads it back reversed, writes the line
   "4 integers, sum 135", and exits with the sum.

   Under rcc and under clang -O2 it prints the line and exits 135:
   nothing here is undefined.

   On x86-64 and on aarch64 the listing must show read_char() under
   .type read_char, @function without .globl, and getch(), ungetch() and
   getint() under .globl too; the input's literal in the literals'
   block after the two formats the functions name, .Ls2; then in .data
   bufp, its .long 0 written, and input, .size input, 8 and
   .quad .Ls2; and in .bss buf, .size buf, 100 and .zero 100, under
   .p2align 4 on x86-64 and none on aarch64; none of the three under
   .globl. */
#include <stdio.h>

#define BUFSIZE 100

static char buf[BUFSIZE]; /* buffer for ungetch */
static int bufp = 0;      /* next free position in buf */

static char *input = " 12 -7  +30\t100\n";

/* the next character of the input, or EOF at its end */
static int
read_char(void)
{
        return *input != '\0' ? *input++ : EOF;
}

/* get a (possibly pushed-back) character */
int
getch(void)
{
        return bufp > 0 ? buf[--bufp] : read_char();
}

/* push character back on input */
void
ungetch(int c)
{
        if (bufp >= BUFSIZE)
                printf("ungetch: too many characters\n");
        else
                buf[bufp++] = c;
}

/* get next integer from input into *pn: 0 at the end of the input */
int
getint(int *pn)
{
        int c, sign;

        while ((c = getch()) == ' ' || c == '\t' || c == '\n')
                ;
        if (c == EOF)
                return 0;
        sign = c == '-' ? -1 : 1;
        if (c == '+' || c == '-')
                c = getch();
        for (*pn = 0; c >= '0' && c <= '9'; c = getch())
                *pn = 10 * *pn + (c - '0');
        *pn *= sign;
        if (c != EOF)
                ungetch(c);
        return c;
}

int
main(void)
{
        int n, sum, count;

        sum = 0;
        count = 0;
        while (getint(&n) != 0) {
                sum += n;
                count++;
        }

        ungetch('a');
        ungetch('b');
        ungetch('c');
        if (getch() != 'c' || getch() != 'b' || getch() != 'a')
                return 1;
        if (getch() != EOF)
                return 2;

        printf("%d integers, sum %d\n", count, sum);

        return sum;
}
