/* Binary &, 6.5.11, in the six shapes of an operator on locals and a
   constant, as addition.c's +, with and and andl.  & commutes, so
   5 & a is written as a & 5.  aarch64's and takes a bitmask alone as
   its immediate, and 5 is none: it is moved into a register of its
   own, mov w1, #5, on either side.

   both() ands two locals, right_constant() and left_constant() a local
   and a constant on either side; nested_left(), nested_right() and
   nested_both() nest an and on the left, on the right and on both. */
int
both(void)
{
        int a = 6;
        int b = 3;
        return a & b;
}

int
right_constant(void)
{
        int a = 6;
        return a & 5;
}

int
left_constant(void)
{
        int a = 6;
        return 5 & a;
}

int
nested_left(void)
{
        int a = 6;
        int b = 3;
        return (a & b) & a;
}

int
nested_right(void)
{
        int a = 6;
        int b = 3;
        return a & (b & a);
}

int
nested_both(void)
{
        int a = 6;
        int b = 3;
        return (a & b) & (a & b);
}
