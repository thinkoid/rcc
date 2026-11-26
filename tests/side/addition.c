/* Binary +, 6.5.7p6, in the six shapes of an operator on locals and a
   constant.  The left operand goes into the register first, and the
   operation is in place on it: a local on the right is the operand in
   memory on x86-64, addl -8(%rbp), %eax, and a register of its own on
   aarch64, add w0, w0, w1; a constant on the right is the immediate,
   addl $5, %eax, add w0, w0, #5.  + commutes, so 5 + a is written as
   a + 5 on both targets.  A nested right operand takes the next
   register while the left one is held: a + (b + a) and
   (a + b) + (a + b) add a second register into the first.

   both() adds two locals, right_constant() and left_constant() a local
   and a constant on either side; nested_left(), nested_right() and
   nested_both() nest a sum on the left, on the right and on both. */
int
both(void)
{
        int a = 6;
        int b = 3;
        return a + b;
}

int
right_constant(void)
{
        int a = 6;
        return a + 5;
}

int
left_constant(void)
{
        int a = 6;
        return 5 + a;
}

int
nested_left(void)
{
        int a = 6;
        int b = 3;
        return (a + b) + a;
}

int
nested_right(void)
{
        int a = 6;
        int b = 3;
        return a + (b + a);
}

int
nested_both(void)
{
        int a = 6;
        int b = 3;
        return (a + b) + (a + b);
}
