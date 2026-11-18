/* K&R's squeeze, section 2.8 of the second edition, and its strcat as
   concat, written from the book's description, each called through a
   declaration in the block that calls it, as the first edition wrote
   its declarations, which a function declared at block scope makes
   possible.  squeeze() deletes each c from s; concat() appends t to s.
   main() declares both, defined after it in the file, and strlen() and
   strcmp() of the C library, whose header the program does not
   include, and text and tail, arrays defined after it; an inner block
   declares abs(), and count() declares strlen() again, the one entity
   of external linkage either names, 6.2.2p2.

   The program squeezes the a's out of "banana bandana", appends
   " cabana", and checks the string with strcmp, its length with
   count(), and abs(n - 100).  It writes the line "bnn bndn cabana 85"
   and exits 85, or exits 100 plus the number of the first check that
   failed.

   Under rcc and under clang -O2 it prints the line and exits 85:
   nothing here is undefined.

   On x86-64 and on aarch64 the listing must show each call as a call
   of a function declared at file scope is written, call squeeze@PLT
   and call strlen@PLT, bl squeeze and bl strlen; and text, which the
   file defines after main, by its label, leaq text(%rip), %rax and
   adrp x0, text; add x0, x0, :lo12:text, as an object the file
   defines is reached wherever it is declared, where one it only
   declares would be reached through the GOT. */
#include <stdio.h>

/* the length of s, by the C library's strlen, declared here again */
static int
count(char s[])
{
        unsigned long strlen(const char *s);

        return (int) strlen(s);
}

int
main(void)
{
        void squeeze(char s[], int c);
        void concat(char s[], char t[]);
        unsigned long strlen(const char *s);
        int strcmp(const char *s, const char *t);
        extern char text[], tail[];
        int n;

        squeeze(text, 'a');
        concat(text, tail);
        n = (int) strlen(text);
        {
                int abs(int j);

                n = abs(n - 100);
        }

        printf("%s %d\n", text, n);

        if (strcmp(text, "bnn bndn cabana") != 0)
                return 101;
        if (count(text) != 15 || count(tail) != 7)
                return 102;
        if (n != 85)
                return 103;

        return n;
}

char text[64] = "banana bandana";
char tail[] = " cabana";

/* s with every c deleted */
void
squeeze(char s[], int c)
{
        int i, j;

        for (i = j = 0; s[i] != '\0'; i++)
                if (s[i] != c)
                        s[j++] = s[i];
        s[j] = '\0';
}

/* t appended to the end of s, which has room for it */
void
concat(char s[], char t[])
{
        int i, j;

        i = j = 0;
        while (s[i] != '\0')
                i++;
        while ((s[i++] = t[j++]) != '\0')
                ;
}
