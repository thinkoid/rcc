/* A comparison as an operand of an arithmetic operator, on either side:
   its value, 1 or 0 in a register, is the operand, and nothing of it
   is a jump, so a value held across it stays in its register.
   a + (b < c) holds a in eax and compares b in ecx, setl %cl and
   movzbl %cl, %ecx, then addl %ecx, %eax; aarch64 holds a in w0 and
   writes cset w1, lt.  (b < c) + a is the comparison's value first,
   with a the add's operand in memory on x86-64.  Under / a divisor
   that is a comparison is set in ecx, outside the eax and edx idivl
   pins, and a dividend that is one is set in eax; under << a count
   that is a comparison is set in ecx, the register a count's consumer
   names.  A division as a comparison's right operand pins eax and edx,
   so the left operand is loaded into ecx, and the setl writes eax, the
   register the return names, once the quotient is consumed.

   Each function returns one arithmetic operator with a comparison on
   one side, its name the operator and the side; divided_compared()
   compares a local with a quotient and remainder_compared() a
   remainder with a local. */
int
sum_right(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return a + (b < c);
}

int
sum_left(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return (b < c) + a;
}

int
difference_right(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return a - (b == c);
}

int
difference_left(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return (b != c) - a;
}

int
quotient_right(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return a / (b <= c);
}

int
quotient_left(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return (b >= c) / a;
}

int
shift_right(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return a << (b > c);
}

int
shift_left(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return (b > c) << a;
}

int
divided_compared(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return a < b / c;
}

int
remainder_compared(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return a % b < c;
}
