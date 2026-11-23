/* An object declared weak after its definition: GNU C lets the later
   declaration make the object weak, which rcc has taken as an object
   of a strong symbol, written after the parse, so the attribute,
   written too late, is unsupported.  A tentative definition, int
   count;, is refused the same way. */
int count = 1;

extern int count __attribute__((__weak__));

int
main(void)
{
        return 0;
}
