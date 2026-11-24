/* A pointer to an array of a typedef through a marking attribute,
   converted to a pointer to an array of long: rcc keeps nothing of
   what mode (word) makes of long, so it cannot tell whether the two
   element types are compatible, and the conversion is unsupported, by
   the attribute's words. */
typedef long word_t __attribute__((mode(word)));

void
keep(word_t (*p)[3])
{
        long (*q)[3] = p;
}

int
main(void)
{
        return 0;
}
