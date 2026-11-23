/* A pointer to a typedef through a marking attribute less a pointer to
   long: rcc keeps nothing of what mode (word) makes of long, so it
   cannot tell whether the two pointees are compatible, and the
   subtraction is unsupported, by the attribute's words. */
typedef long word_t __attribute__((mode(word)));

long
distance(word_t *w, long *l)
{
        return w - l;
}

int
main(void)
{
        return 0;
}
