/* Shifts under and over + and *.  On x86-64 a shift by a count that is
   no constant pins rcx: the left operand of a node whose right operand
   is such a shift is loaded outside rcx, so a + (b << a) holds a in
   eax, shifts b in edx by a in ecx and adds edx; a shift by a constant
   pins nothing, and a + (b << 2) has the rows of a + (b + 2), b in ecx
   and shll $2, %ecx.  A value held in ecx where the count goes moves
   out of the way, all 64 bits: a + (b + (c + (d << a))) holds b in
   ecx, loads the count into edi, then movq %rcx, %r8 and movl %edi,
   %ecx.  A shift on the left of + or * is its row, then the operation;
   a sum or a product shifted is its row, then the shift.  aarch64 pins
   nothing, and every shape is that of +.

   Each function returns one such expression, its name what it
   shows. */
int
add_shl(void)
{
        int a = 3;
        int b = 5;
        return a + (b << a);
}

int
add_shl_constant(void)
{
        int a = 3;
        int b = 5;
        return a + (b << 2);
}

int
shl_add(void)
{
        int a = 3;
        int b = 5;
        return (a << b) + (a << b);
}

int
sum_shl(void)
{
        int a = 3;
        int b = 5;
        return (a + b) << 3;
}

int
sum_shl_local(void)
{
        int a = 3;
        int b = 5;
        return (a + b) << a;
}

int
sub_shr(void)
{
        int a = 3;
        int b = 500;
        return a - (b >> a);
}

int
constant_sub_shl(void)
{
        int a = 3;
        return 7 - (a << a);
}

int
constant_add_shl(void)
{
        int a = 3;
        int b = 5;
        return 7 + (b << a);
}

int
mul_shl(void)
{
        int a = 3;
        int b = 5;
        return a * (b << a);
}

int
shl_mul(void)
{
        int a = 3;
        int b = 5;
        return (a << b) * 5;
}

int
constant_mul_shl(void)
{
        int a = 3;
        int b = 5;
        return 5 * (a << b);
}

int
product_shl(void)
{
        int a = 3;
        int b = 5;
        return (a * b) << a;
}

int
shl_by_product(void)
{
        int a = 3;
        int b = 5;
        return a << (b * a);
}

int
negation_shl(void)
{
        int a = 3;
        int b = 5;
        return -a << ~b;
}

int
xor_shr(void)
{
        int a = 3;
        int b = 500;
        return (a ^ b) >> (b & a);
}

int
count_vacates(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        int d = 4;
        return a + (b + (c + (d << a)));
}
