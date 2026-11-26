/* Simple assignment, 6.5.17.2.  An assignment whose value is dropped is
   the store of an initializer, rcc's initialize(): on x86-64 a constant
   is one movl into the slot, on aarch64 0 is the store of wzr, and
   anything else goes through a register.  An assignment whose value is
   used, return x = 7;, puts the value in the register first and stores
   it from there, a constant and 0 alike.

   constant(), zero() and copy() store 7, 0 and another local, the value
   dropped; their twins used_constant(), used_zero() and used_copy()
   return the assignment.  chain() and chain_zero() store one value in
   two locals, x = y = 7;, the innermost store first, so even 0 goes
   through a register; initialized() initializes y from an assignment;
   itself() stores x into x. */
int
constant(void)
{
        int x;
        x = 7;
        return x;
}

int
zero(void)
{
        int x;
        x = 0;
        return x;
}

int
copy(void)
{
        int x = 7;
        int y;
        y = x;
        return y;
}

int
used_constant(void)
{
        int x;
        return x = 7;
}

int
used_zero(void)
{
        int x;
        return x = 0;
}

int
used_copy(void)
{
        int x = 7;
        int y;
        return y = x;
}

int
chain(void)
{
        int x;
        int y;
        x = y = 7;
        return x;
}

int
chain_zero(void)
{
        int x;
        int y;
        x = y = 0;
        return y;
}

int
initialized(void)
{
        int x;
        int y = x = 3;
        return y;
}

int
itself(void)
{
        int x = 5;
        x = x;
        return x;
}
