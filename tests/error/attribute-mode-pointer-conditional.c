/* A conditional whose arms are a pointer to a typedef through a
   marking attribute and a pointer to long: rcc keeps nothing of what
   mode (word) makes of long, so it cannot tell whether the two
   pointees are compatible, and the arms' join is not yet, by the
   attribute's words. */
typedef long word_t __attribute__((mode(word)));

long *
either(int c, word_t *w, long *l)
{
        return c ? w : l;
}

int
main(void)
{
        return 0;
}
