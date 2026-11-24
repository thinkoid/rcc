/* A conditional whose arms are a pointer to a function whose result is
   of a typedef through a marking attribute and a pointer to a function
   whose result is long: rcc keeps nothing of what mode (word) makes of
   long, so it cannot tell whether the two function types are
   compatible, and the arms' join is unsupported, by the attribute's
   words. */
typedef long word_t __attribute__((mode(word)));

long (*either(int c, word_t (*p)(void), long (*q)(void)))(void)
{
        return c ? p : q;
}

int
main(void)
{
        return 0;
}
