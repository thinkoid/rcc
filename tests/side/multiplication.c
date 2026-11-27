/* Binary *, 6.5.6p5, in the six shapes of an operator on locals and a
   constant, as addition.c's +, with mul and imull.  * commutes, so
   5 * a is written as a * 5 on both targets.  x86-64's imull by a
   constant has three operands: a local on its left is the source in
   memory, imull $5, -4(%rbp), %eax, and no load; any other left
   operand is its own row first, then imull $5, %eax, %eax.  aarch64's
   mul takes no immediate, so the constant is moved into a register of
   its own, mov w1, #5, on either side.  Under + and - a product is the
   right operand, in the next register while the left one is held.

   both() multiplies two locals, right_constant() and left_constant() a
   local and a constant on either side; nested_left(), nested_right()
   and nested_both() nest a product on the left, on the right and on
   both.  sum_constant(), constant_sum() and negation_constant()
   multiply a node by a constant on either side; difference_product()
   and product_sum() mix * with - and +, and sum_product() is
   a + (b * a) by precedence. */
int
both(void)
{
        int a = 6;
        int b = 3;
        return a * b;
}

int
right_constant(void)
{
        int a = 6;
        return a * 5;
}

int
left_constant(void)
{
        int a = 6;
        return 5 * a;
}

int
nested_left(void)
{
        int a = 6;
        int b = 3;
        return (a * b) * a;
}

int
nested_right(void)
{
        int a = 6;
        int b = 3;
        return a * (b * a);
}

int
nested_both(void)
{
        int a = 6;
        int b = 3;
        return (a * b) * (a * b);
}

int
sum_constant(void)
{
        int a = 6;
        int b = 3;
        return (a + b) * 5;
}

int
constant_sum(void)
{
        int a = 6;
        int b = 3;
        return 5 * (a + b);
}

int
negation_constant(void)
{
        int a = 6;
        return -a * 5;
}

int
difference_product(void)
{
        int a = 6;
        int b = 3;
        return a - b * 5;
}

int
product_sum(void)
{
        int a = 6;
        int b = 3;
        return a * 5 + b;
}

int
sum_product(void)
{
        int a = 6;
        int b = 3;
        return a + b * a;
}
