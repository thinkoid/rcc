/* The definition of a function declared weak: rcc would have to define
   a weak symbol, which is unsupported. */
int hook(void) __attribute__((__weak__));

int
hook(void)
{
        return 0;
}
