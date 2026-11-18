/* K&R's count of the words of a text in a binary tree, section 6.5 of
   the second edition, written from the book's description: each word
   is a node of a self-referential struct, its count and the two
   subtrees of the words before it and after it, made by malloc and
   reached through ->.  The program reads a string, not its standard
   input, so that it runs alone: a word is a run of letters, and the
   tree is printed in order, each word with its count.

   The text has 20 words, 14 of them distinct: "the" four times, "of"
   twice, "a" and "tree" twice each.  It prints the words in order and
   exits with the count of distinct words times 3 plus the count of
   "the", 14 * 3 + 4 = 46, or 100 plus the number of the first check
   that failed.

   Under rcc and under clang -O2 it prints the same lines and exits 46:
   nothing here is undefined. */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* a word, how many times it is met, and the words before and after */
struct tnode
{
        char *word;
        int count;
        struct tnode *left;
        struct tnode *right;
};

/* the text the program counts */
static char text[] = "the shape of a tree: the root, the nodes of the "
                     "tree, a word and its count, left or right";

static int at;

/* the next word of the text into word, or 0 at its end */
static int
getword(char *word, int limit)
{
        char *w = word;

        while (text[at] != '\0' && !isalpha((unsigned char) text[at]))
                at++;

        if (text[at] == '\0')
                return 0;

        while (isalpha((unsigned char) text[at]) && --limit > 0)
                *w++ = text[at++];

        *w = '\0';

        return 1;
}

/* the node of w in the tree at p, made if there is none */
static struct tnode *
addtree(struct tnode *p, char *w)
{
        int cond;

        if (p == NULL) {
                p = malloc(sizeof *p);
                p->word = strdup(w);
                p->count = 1;
                p->left = p->right = NULL;
        }
        else if ((cond = strcmp(w, p->word)) == 0)
                p->count++;
        else if (cond < 0)
                p->left = addtree(p->left, w);
        else
                p->right = addtree(p->right, w);

        return p;
}

/* the words of the tree in order, and the count of the nodes */
static int
treeprint(struct tnode *p)
{
        if (p == NULL)
                return 0;

        int n = treeprint(p->left);
        printf("%4d %s\n", p->count, p->word);

        return n + 1 + treeprint(p->right);
}

/* the count of the word w */
static int
count(struct tnode *p, const char *w)
{
        while (p != NULL) {
                int cond = strcmp(w, p->word);
                if (cond == 0)
                        return p->count;

                p = cond < 0 ? p->left : p->right;
        }

        return 0;
}

int
main(void)
{
        struct tnode *root = NULL;
        char word[100];
        int words = 0;

        while (getword(word, sizeof word)) {
                root = addtree(root, word);
                words++;
        }

        int distinct = treeprint(root);

        if (words != 20)
                return 101;
        if (count(root, "of") != 2 || count(root, "tree") != 2)
                return 102;
        if (count(root, "a") != 2 || count(root, "count") != 1)
                return 103;

        return distinct * 3 + count(root, "the");
}
