/* A static function called, written without .globl, by call f@PLT and
   bl f as any other; one declared static, called, and defined after the
   call is written where it was first declared. */
static int later(void);

static int
earlier(void)
{
        return 1;
}

int
f(void)
{
        return earlier() + later();
}

static int
later(void)
{
        return 2;
}
