/* Binary -, 6.5.7p7, in the six shapes of an operator on locals and a
   constant, as addition.c's +, with sub and subl.  - does not commute,
   so 5 - a keeps its order: 5 into the register first, movl $5, %eax
   and mov w0, #5, then the local subtracted from it.

   both() subtracts two locals, right_constant() and left_constant() a
   local and a constant on either side; nested_left(), nested_right()
   and nested_both() nest a difference on the left, on the right and
   on both. */
int
both(void)
{
        int a = 6;
        int b = 3;
        return a - b;
}

int
right_constant(void)
{
        int a = 6;
        return a - 5;
}

int
left_constant(void)
{
        int a = 6;
        return 5 - a;
}

int
nested_left(void)
{
        int a = 6;
        int b = 3;
        return (a - b) - a;
}

int
nested_right(void)
{
        int a = 6;
        int b = 3;
        return a - (b - a);
}

int
nested_both(void)
{
        int a = 6;
        int b = 3;
        return (a - b) - (a - b);
}
