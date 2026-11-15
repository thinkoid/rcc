/* K&R's itoa and reverse, section 3.6 of the second edition, and its
   trim, section 3.7, written from the book's description, which do,
   break and continue make possible.  itoa() writes the digits of n
   into s, the last first, in a do loop, so that 0 still writes one
   digit; then the sign, and reverses the string.  reverse() swaps the
   ends of s towards its middle in a for with two indices.  trim()
   walks back from the end of s past blanks, tabs and newlines, leaves
   at the first other character by break, and ends s after it; it
   returns the index of that character, -1 for a string of blanks.
   count_digits() counts the digits of a string, skipping each other
   character by continue.

   The program converts 0, 7, -7, 1234, -98765 and 2147483647, reverses
   "abc" and the empty string, trims "hello \t\n", "   " and "x", counts
   the digits of "a1b22c333", and checks each result with strcmp or ==;
   the strings are static arrays, since a local array's initializer is
   not yet.  It writes the line "-98765 2147483647 [hello]" and exits
   100, or exits with the number of the first check that failed.
   INT_MIN is not converted: -n overflows there, as K&R's exercise 3-4
   says.

   Under rcc and under clang -O2 it prints the line and exits 100:
   nothing here is undefined.

   On x86-64 and on aarch64 the listing must show itoa()'s do loop, the
   body under the head's label, .Litoa.3, and the condition at the
   bottom with the jump back to it, jg .Litoa.3 on x86-64 and
   b.gt .Litoa.3 on aarch64; trim()'s break, jmp .Ltrim.4 or
   b .Ltrim.4, to the label past its for; and count_digits()'s
   continue, jmp .Lcount_digits.3 or b .Lcount_digits.3, to the for's
   step. */
#include <stdio.h>
#include <string.h>

/* s reversed in place */
void
reverse(char s[])
{
        int c, i, j;

        for (i = 0, j = strlen(s) - 1; i < j; i++, j--) {
                c = s[i];
                s[i] = s[j];
                s[j] = c;
        }
}

/* the decimal digits of n into s, a sign first if n is negative */
void
itoa(int n, char s[])
{
        int i, sign;

        if ((sign = n) < 0)
                n = -n;
        i = 0;
        do {
                s[i++] = n % 10 + '0';
        } while ((n /= 10) > 0);
        if (sign < 0)
                s[i++] = '-';
        s[i] = '\0';
        reverse(s);
}

/* s without its trailing blanks, tabs and newlines; the index of its
   last other character */
int
trim(char s[])
{
        int n;

        for (n = strlen(s) - 1; n >= 0; n--)
                if (s[n] != ' ' && s[n] != '\t' && s[n] != '\n')
                        break;
        s[n + 1] = '\0';
        return n;
}

/* the count of the digits of s */
int
count_digits(char s[])
{
        int i, n;

        n = 0;
        for (i = 0; s[i] != '\0'; i++) {
                if (s[i] < '0' || s[i] > '9')
                        continue;
                n++;
        }
        return n;
}

/* the strings the checks write in, each its own array */
static char word[] = "hello \t\n";
static char blanks[] = "   ";
static char x[] = "x";
static char abc[] = "abc";
static char empty[] = "";
static char digits[] = "a1b22c333";

int
main(void)
{
        char a[16], b[16], c[16];

        itoa(0, a);
        if (strcmp(a, "0") != 0)
                return 1;
        itoa(7, a);
        if (strcmp(a, "7") != 0)
                return 2;
        itoa(-7, a);
        if (strcmp(a, "-7") != 0)
                return 3;
        itoa(1234, a);
        if (strcmp(a, "1234") != 0)
                return 4;
        itoa(-98765, b);
        if (strcmp(b, "-98765") != 0)
                return 5;
        itoa(2147483647, c);
        if (strcmp(c, "2147483647") != 0)
                return 6;

        reverse(abc);
        if (strcmp(abc, "cba") != 0)
                return 7;
        reverse(empty);
        if (strcmp(empty, "") != 0)
                return 8;

        if (trim(word) != 4 || strcmp(word, "hello") != 0)
                return 9;
        if (trim(blanks) != -1 || strcmp(blanks, "") != 0)
                return 10;
        if (trim(x) != 0 || strcmp(x, "x") != 0)
                return 11;

        if (count_digits(digits) != 6)
                return 12;

        printf("%s %s [%s]\n", b, c, word);

        return 100;
}
