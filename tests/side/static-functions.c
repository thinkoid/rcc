/* A function declared static has internal linkage, 6.2.2p3, and its
   symbol is written without .globl, used or not.  A declaration
   without a storage class after a static one gives the function the
   earlier declaration's linkage, p4, p5, so plain, declared static
   and defined without it, and later, defined static and declared
   without it, are static too; twice is declared and defined static.
   A static function only declared writes nothing. */
static int never(void);

static int
unused(void)
{
        return 1;
}

static int plain(void);

int
plain(void)
{
        return 2;
}

static int
later(void)
{
        return 3;
}

int later(void);

static int twice(void);

static int
twice(void)
{
        return 4;
}

int
main(void)
{
        return 0;
}
