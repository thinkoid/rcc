/* A call as a condition and in one.  A call leaves no record of ZF, so
   its value is compared with 0, cmpl $0, %eax and je, cbz w0 on
   aarch64; ! and == 0 flip the jump's sense; a comparison with a call
   on its right spills the left operand at the call and reloads it,
   movq -24(%rbp), %rcx and cmpl %eax, %ecx; an operator whose own
   instruction sets ZF jumps on it, addl $1, %eax and je, but imul sets
   none and is compared; & is test, testl $1, %eax; a store withdraws
   the record.  && and || test each operand's call where it stands, the
   second call's slot taken whether a path reaches it or not. */
int g(int a);
int h(int a);

int
value(int a)
{
        if (g(a))
                return 1;
        return 0;
}

int
negated(int a)
{
        if (!g(a))
                return 1;
        if (g(a) == 0)
                return 2;
        if (g(a) != 0)
                return 3;
        return 0;
}

int
compared(int a, int b)
{
        if (g(a) > 5)
                return 1;
        if (g(a) < b)
                return 2;
        if (a < g(b))
                return 3;
        if (g(a) > h(b))
                return 4;
        return 0;
}

int
flags(int a, int b)
{
        if (g(a) + 1)
                return 1;
        if (a + g(b))
                return 2;
        if (g(a) - b)
                return 3;
        if (g(a) & 1)
                return 4;
        if (g(a) * 2)
                return 5;
        if (a = g(b))
                return 6;
        return 0;
}

int
logical(int a, int b)
{
        if (g(a) && h(b))
                return 1;
        if (g(a) || h(b))
                return 2;
        if (!!g(a))
                return 3;
        return 0;
}

int
values(int a, int b)
{
        int x = !g(a);
        int y = g(a) && h(b);
        int z = g(a) || h(b);
        return x + y + z + (g(a) < b) + (g(a) == 0) + (a && g(b));
}
