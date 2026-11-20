/* cost.md chapter 1, alias: GNU C's alias attribute declares a symbol
   of another's address, which the assembler gives it, so it costs no
   instruction.  bgcmd() is the alias of fgcmd(), which the unit
   defines, as a shell names one builtin by two names; total is the
   alias of count.  The aliases follow the data, as clang writes them.

   On x86-64 the listing must hold, after the data, .globl bgcmd,
   .type bgcmd, @function and bgcmd = fgcmd, then .globl total and
   total = count, and nothing else of the aliases.

   On aarch64 the listing must hold the same lines. */
int
fgcmd(void)
{
        return 0;
}

int bgcmd(void) __attribute__((__alias__("fgcmd")));

int count = 1;
extern int total __attribute__((alias("count")));
