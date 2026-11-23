/* A function declared weak after its definition: GNU C lets the later
   declaration make the definition weak, which rcc has taken as the
   definition of a strong symbol, so the attribute, written too late,
   is unsupported. */
int
hook(void)
{
        return 0;
}

int hook(void) __attribute__((__weak__));

int
main(void)
{
        return 0;
}
