/* A weak alias of internal linkage, which GNU C refuses: a weak symbol
   is one another file's definition may stand in for, and no other file
   sees a symbol of internal linkage. */
int
fgcmd(void)
{
        return 0;
}

static int bgcmd(void) __attribute__((weak, alias("fgcmd")));
