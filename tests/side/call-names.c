/* aarch64's bl quotes a callee's name that the assembler would read as
   a register's, in any case, bl "x1", "W0", "sp", "lr", "nzcv", for a
   function only declared too; x32 names no register.  The name is
   unquoted in .globl, .type, .size and the label, and on x86-64. */
int x1(void);
int W0(void);
int x2(int a);
int x32(void);

int
sp(void)
{
        return 1;
}

int
lr(void)
{
        return 2;
}

int
nzcv(void)
{
        return 3;
}

int
f(void)
{
        return x1() + W0() + x2(1) + x32() + sp() + lr() + nzcv();
}
