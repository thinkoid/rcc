/* K&R's keyword count, section 6.3 of the second edition, written from
   the book's description: a table of the keywords of C in alphabetical
   order, each entry a struct of the keyword and its count, initialized
   at file scope by one list without braces for the entries, whose
   last count no initializer gives; a binary search finds a word in it,
   and the program counts the keywords of a text.

   The text is a fragment of C, read word by word from an array.  The
   program prints each keyword it counted with its count and exits with
   the sum of the counts, 14, int's 3 and eleven others' 1, plus the
   count of the keywords counted, 12, or 200 plus the number of the
   first check that failed.

   Under rcc and under clang -O2 it prints the same lines and exits 26:
   nothing here is undefined. */
#include <ctype.h>
#include <stdio.h>
#include <string.h>

#define MAXWORD 32

/* a keyword and its count */
struct key
{
        char *word;
        int count;
};

/* the keywords, in alphabetical order */
static struct key keytab[] = {
        "auto",     0, "break",   0, "case",   0, "char",     0, "const",  0,
        "continue", 0, "default", 0, "do",     0, "double",   0, "else",   0,
        "enum",     0, "extern",  0, "float",  0, "for",      0, "goto",   0,
        "if",       0, "int",     0, "long",   0, "register", 0, "return", 0,
        "short",    0, "signed",  0, "sizeof", 0, "static",   0, "struct", 0,
        "switch",   0, "typedef", 0, "union",  0, "unsigned", 0, "void",   0,
        "volatile", 0, "while",
};

#define NKEYS (sizeof keytab / sizeof keytab[0])

/* the text whose keywords are counted */
static char text[] = "static int count(const char *s)\n"
                     "{\n"
                     "        int n = 0;\n"
                     "        while (*s != '\\0')\n"
                     "                if (*s++ == ' ')\n"
                     "                        n++;\n"
                     "                else\n"
                     "                        continue;\n"
                     "        for (;;)\n"
                     "                break;\n"
                     "        return (int) sizeof n + n;\n"
                     "}\n";

/* the place in text of the next word */
static char *next = text;

/* the index of word in tab[0] to tab[n - 1], or -1 */
static int
binsearch(const char *word, struct key tab[], int n)
{
        int low = 0;
        int high = n - 1;

        while (low <= high) {
                int mid = (low + high) / 2;
                int cond = strcmp(word, tab[mid].word);

                if (cond < 0)
                        high = mid - 1;
                else if (cond > 0)
                        low = mid + 1;
                else
                        return mid;
        }

        return -1;
}

/* the next word of the text, or the next character that is not a
   letter, into word; its first character, or 0 at the text's end */
static int
getword(char *word, int lim)
{
        char *w = word;

        while (isspace((unsigned char) *next))
                next++;

        if (*next == '\0')
                return 0;

        *w++ = *next;
        if (!isalpha((unsigned char) *next++)) {
                *w = '\0';
                return word[0];
        }

        for (; --lim > 0 && isalnum((unsigned char) *next); next++)
                *w++ = *next;
        *w = '\0';

        return word[0];
}

int
main(void)
{
        char word[MAXWORD];
        int n;

        if (NKEYS != 32)
                return 201;
        if (keytab[31].count != 0 || strcmp(keytab[31].word, "while") != 0)
                return 202;

        while (getword(word, MAXWORD) != 0)
                if (isalpha((unsigned char) word[0]))
                        if ((n = binsearch(word, keytab, NKEYS)) >= 0)
                                keytab[n].count++;

        int sum = 0;
        int counted = 0;
        for (n = 0; n < (int) NKEYS; n++)
                if (keytab[n].count > 0) {
                        printf("%4d %s\n", keytab[n].count, keytab[n].word);
                        sum += keytab[n].count;
                        counted++;
                }

        if (binsearch("main", keytab, NKEYS) != -1)
                return 203;

        return sum + counted;
}
