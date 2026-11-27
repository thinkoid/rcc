/* Binary /, 6.5.6p6, p7, in the six shapes of an operator on locals and
   a constant.  x86-64's idivl takes the dividend in eax and writes the
   quotient there: the dividend is loaded into eax, cltd fills edx with
   its sign, and a local divisor is the instruction's operand in memory,
   idivl -8(%rbp).  idivl takes no immediate, so a constant divisor is
   moved into a register outside eax and edx, movl $5, %ecx.  A
   divisor that is a node is computed first, then moved out of eax,
   all 64 bits, movq %rax, %rsi, and the dividend, which was loaded
   away from eax and edx, moved in.  aarch64's sdiv names any registers,
   sdiv w0, w0, w1, and takes no immediate either.  / does not commute,
   so 5 / a keeps its order.

   both() divides a local by a local, right_constant() by a constant,
   left_constant() a constant by a local; nested_left(), nested_right()
   and nested_both() nest a division on the left, on the right and on
   both. */
int
both(void)
{
        int a = 60;
        int b = 7;
        return a / b;
}

int
right_constant(void)
{
        int a = 60;
        return a / 5;
}

int
left_constant(void)
{
        int a = 7;
        return 500 / a;
}

int
nested_left(void)
{
        int a = 60;
        int b = 3;
        return (a / b) / a;
}

int
nested_right(void)
{
        int a = 6;
        int b = 60;
        return a / (b / a);
}

int
nested_both(void)
{
        int a = 60;
        int b = 3;
        return (a / b) / (a / b);
}

int
negative(void)
{
        int a = -7;
        int b = 2;
        return a / b;
}
