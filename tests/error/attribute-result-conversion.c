/* A pointer to a function whose result is of a typedef through a
   marking attribute, assigned to a pointer to a function of long: rcc
   keeps nothing of what mode (word) makes of long, so it cannot tell
   whether the two function types are compatible, and the conversion is
   unsupported, by the attribute's words, as the cast is. */
typedef long word_t __attribute__((mode(word)));

long (*plain)(void);

void
keep(word_t (*p)(void))
{
        plain = p;
}

int
main(void)
{
        return 0;
}
