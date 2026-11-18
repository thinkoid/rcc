/* K&R's table lookup, section 6.6 of the second edition, written from
   the book's description: a hash table of the names and replacement
   texts a macro processor keeps, each entry a struct in a chain of the
   entries of one hash, which a global array of pointers heads.  install
   puts a name in the table or replaces its text, lookup finds it.

   The program installs seven names, replaces the text of one, prints
   each name with its text, and exits with the sum of the lengths of the
   texts found, 3 + 1 + 3 + 4 + 2 + 6 + 3 = 22, plus 10 times the count
   of the chains of more than one entry, or 100 plus the number of the
   first check that failed.  The seven names hash to six chains of the
   table of 101, so one chain holds two entries and the status is 32.

   Under rcc and under clang -O2 it prints the same lines and exits 32:
   nothing here is undefined. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HASHSIZE 101

/* an entry: the next one of its chain, the name and its text */
struct nlist
{
        struct nlist *next;
        char *name;
        char *defn;
};

/* the table, a chain of entries for each hash */
static struct nlist *hashtab[HASHSIZE];

/* the hash of a string */
static unsigned
hash(const char *s)
{
        unsigned hashval;

        for (hashval = 0; *s != '\0'; s++)
                hashval = *s + 31 * hashval;

        return hashval % HASHSIZE;
}

/* the entry of s, or NULL */
static struct nlist *
lookup(const char *s)
{
        struct nlist *np;

        for (np = hashtab[hash(s)]; np != NULL; np = np->next)
                if (strcmp(s, np->name) == 0)
                        return np;

        return NULL;
}

/* name with its text defn, in the table; NULL when there is no room */
static struct nlist *
install(const char *name, const char *defn)
{
        struct nlist *np = lookup(name);

        if (np == NULL) {
                np = malloc(sizeof *np);
                if (np == NULL || (np->name = strdup(name)) == NULL)
                        return NULL;

                unsigned hashval = hash(name);
                np->next = hashtab[hashval];
                hashtab[hashval] = np;
        }
        else
                free(np->defn);

        if ((np->defn = strdup(defn)) == NULL)
                return NULL;

        return np;
}

/* the names installed, in order */
static const char *names[] = { "IN", "OUT", "MAX", "EOF", "TAB", "YES", "NO" };

int
main(void)
{
        install("IN", "1");
        install("OUT", "0");
        install("MAX", "100");
        install("EOF", "(-1)");
        install("TAB", "\\t");
        install("YES", "always");
        install("NO", "nil");
        install("IN", "one");

        int length = 0;
        for (int i = 0; i < 7; i++) {
                struct nlist *np = lookup(names[i]);
                if (np == NULL)
                        return 101;

                printf("%s %s\n", np->name, np->defn);
                length += (int) strlen(np->defn);
        }

        if (lookup("OTHER") != NULL)
                return 102;
        if (strcmp(lookup("IN")->defn, "one") != 0)
                return 103;

        int long_chains = 0;
        for (int h = 0; h < HASHSIZE; h++)
                if (hashtab[h] != NULL && hashtab[h]->next != NULL)
                        long_chains++;

        return length + 10 * long_chains;
}
