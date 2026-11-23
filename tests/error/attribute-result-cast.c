/* A pointer to a function whose result is of a typedef through a
   marking attribute, cast to a pointer to a function of long: rcc keeps
   nothing of what mode (word) makes of long, and a call through the
   cast pointer would take the result as a long, so the cast is
   unsupported, by the attribute's words. */
typedef long word_t __attribute__((mode(word)));

long (*plain)(void);

void
keep(word_t (*p)(void))
{
        plain = (long (*)(void)) p;
}

int
main(void)
{
        return 0;
}
