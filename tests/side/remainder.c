/* Binary %, 6.5.6p6, p7, in the six shapes of division.c's /.  On
   x86-64 the remainder is the division's, which idivl leaves in edx: a
   new value there, which a return moves into eax, movl %edx, %eax.  A
   remainder held while another division is written moves out of edx
   first, movq %rdx, %rcx.  aarch64 has no instruction for it: the
   quotient into a register taken outside both operands', then msub,
   sdiv w2, w0, w1 and msub w0, w2, w1, w0.

   both() takes a local's remainder by a local, right_constant() by a
   constant, left_constant() a constant's by a local; nested_left(),
   nested_right() and nested_both() nest a remainder on the left, on
   the right and on both. */
int
both(void)
{
        int a = 60;
        int b = 7;
        return a % b;
}

int
right_constant(void)
{
        int a = 61;
        return a % 5;
}

int
left_constant(void)
{
        int a = 7;
        return 500 % a;
}

int
nested_left(void)
{
        int a = 60;
        int b = 7;
        return (a % b) % a;
}

int
nested_right(void)
{
        int a = 7;
        int b = 60;
        return a % (b % a);
}

int
nested_both(void)
{
        int a = 60;
        int b = 7;
        return (a % b) % (a % b);
}

int
negative(void)
{
        int a = -7;
        int b = 2;
        return a % b;
}
