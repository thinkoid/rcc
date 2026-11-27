/* The constants a comparison takes as its immediate, at the edges of
   what aarch64's cmp encodes: the immediate of add and sub, 0 to 4095,
   or that shifted left by 12, of either sign, written signed,
   cmp w0, #4096 and cmp w0, #-1.  A constant cmp does not encode is
   moved into a register of its own after the left operand, mov w1,
   #4097, and the cmp takes the register.  x86-64 takes every int as
   cmp's immediate, against the local in place, cmpl $4097, -4(%rbp),
   the most negative int written as it is.

   Each function returns a local compared with one constant, its name
   the constant. */
int
less_4095(void)
{
        int a = 7;
        return a < 4095;
}

int
less_4096(void)
{
        int a = 7;
        return a < 4096;
}

int
less_4097(void)
{
        int a = 7;
        return a < 4097;
}

int
less_8192(void)
{
        int a = 7;
        return a < 8192;
}

int
less_minus_1(void)
{
        int a = 7;
        return a < -1;
}

int
less_minus_4096(void)
{
        int a = 7;
        return a < -4096;
}

int
less_minimum(void)
{
        int a = 7;
        return a < -2147483647 - 1;
}

int
equal_minimum(void)
{
        int a = 7;
        return a == -2147483647 - 1;
}
