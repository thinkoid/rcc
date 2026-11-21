/* A pointer to int passed for a pointer to a typedef through a marking
   attribute: rcc keeps nothing of what mode (word) makes of int, so it
   cannot tell whether the two pointees are compatible, and the
   conversion is not yet, by the attribute's words. */
typedef int word_t __attribute__((mode(word)));

void f(word_t *w);

void
g(int *p)
{
        f(p);
}

int
main(void)
{
        return 0;
}
