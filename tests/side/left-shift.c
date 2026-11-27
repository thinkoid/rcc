/* Binary <<, 6.5.8p4, in the six shapes of an operator on locals and a
   constant.  A constant count is the instruction's immediate, shll $5,
   %eax and lsl w0, w0, #5.  A count that is no constant is held, never
   the operand in memory: x86-64's instruction takes it in cl, so the
   shifted value is loaded outside rcx and the count into ecx, shll %cl,
   %eax; aarch64 names it in any register, lsl w0, w0, w1.  << does not
   commute, so 5 << a keeps its order: movl $5, %eax.  A count that is
   a node is its row, then moved into ecx: a << (b << a) shifts b in
   edx, by a in ecx, and moves the result there, movl %edx, %ecx.

   both() shifts a local by a local, right_constant() by a constant,
   left_constant() a constant by a local; nested_left(), nested_right()
   and nested_both() nest a shift on the left, on the right and on
   both. */
int
both(void)
{
        int a = 6;
        int b = 3;
        return a << b;
}

int
right_constant(void)
{
        int a = 6;
        return a << 5;
}

int
left_constant(void)
{
        int a = 6;
        return 5 << a;
}

int
nested_left(void)
{
        int a = 6;
        int b = 3;
        return (a << b) << a;
}

int
nested_right(void)
{
        int a = 2;
        int b = 3;
        return a << (b << a);
}

int
nested_both(void)
{
        int a = 2;
        int b = 1;
        return (a << b) << (a << b);
}
