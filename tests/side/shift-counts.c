/* The count of a shift.  A constant count is the immediate as each
   target encodes it: x86-64 writes the low 8 bits as written, shll
   $33, %eax, shll $255, %eax for -1 and shll $5, %eax for 261, which
   the machine masks to 5 bits (G3); aarch64 masks it to the width, lsl
   w0, w0, #1, #31 and #5.  A count that is 0 modulo 32 makes the
   operator the identity, in identities.c.  A count that is a node is
   computed in the register the load of a count takes, ecx, so nothing
   is moved for a << (b + 1).  ecx is the register the count's consumer
   names: a count whose operator commutes and whose left operand is a
   shift, held outside ecx, is written into its right operand's
   register where that is ecx, (0 << d) + (d = c) as addl %edx, %ecx,
   and not into edx and moved.

   Each function returns a local shifted by one count, its name the
   operator and the count. */
int
shl_31(void)
{
        int a = 1;
        return a << 31;
}

int
shr_31(void)
{
        int a = -1;
        return a >> 31;
}

int
shl_33(void)
{
        int a = 1;
        return a << 33;
}

int
shr_37(void)
{
        int a = -64;
        return a >> 37;
}

int
shl_minus_one(void)
{
        int a = 1;
        return a << -1;
}

int
shr_minus_one(void)
{
        int a = -64;
        return a >> -1;
}

int
shl_261(void)
{
        int a = 1;
        return a << 261;
}

int
shl_sum(void)
{
        int a = 3;
        int b = 1;
        return a << (b + 1);
}

int
shr_difference(void)
{
        int a = -96;
        int b = 4;
        return a >> (b - 1);
}

int
shl_negation(void)
{
        int a = 3;
        int b = -2;
        return a << -b;
}

int
shl_product(void)
{
        int a = 3;
        int b = 2;
        return a << b * 2;
}

int
shl_sum_into_ecx(void)
{
        int a = 3;
        int c = 7;
        int d = 11;
        return d << ((0 << d) + (d = c));
}

int
shl_product_into_ecx(void)
{
        int b = 5;
        int c = 7;
        int d = 11;
        return 2 << (((d - d) << (b * c)) * (d = 255 & b));
}

int
shr_and_into_ecx(void)
{
        int a = 3;
        int b = 5;
        int d = 11;
        return (b << d) >> ((a << a) & (b * d));
}
