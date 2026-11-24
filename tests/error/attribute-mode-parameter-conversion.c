/* A pointer to a function whose parameter is of a typedef through a
   marking attribute, assigned to a pointer to a function of a long
   parameter: rcc keeps nothing of what mode (word) makes of long, so it
   cannot tell whether the two function types are compatible, and the
   conversion is unsupported, by the attribute's words. */
typedef long word_t __attribute__((mode(word)));

void (*plain)(long);

void
keep(void (*p)(word_t))
{
        plain = p;
}

int
main(void)
{
        return 0;
}
