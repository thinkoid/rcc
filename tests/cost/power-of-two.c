/* backend.md 6.4 and aarch64.md 6.4, x * 4, and x / 4 and x % 4 on an
   unsigned x: a constant power of two is an operand like any other, so
   the product is a multiply and the quotient and the remainder a
   division, with no shift and no mask, since no row looks at a
   constant's value.  On x86-64 times_four() is imull $4, -4(%rbp),
   %eax, x's slot the source and no load; quarter() loads x into eax,
   then movl $4, %ecx, since div takes no immediate, xorl %edx, %edx and
   divl %ecx, the quotient in eax; and quarter_rest() the same, then
   movl %edx, %eax, the remainder.  On aarch64 times_four() loads x
   into w0, then mov w1, #4, since mul takes no immediate, and mul w0,
   w0, w1; quarter() mov w1, #4 and udiv w0, w0, w1; and quarter_rest()
   mov w1, #4, udiv w2, w0, w1 and msub w0, w2, w1, w0, the quotient
   multiplied back and subtracted. */
int
times_four(int x)
{
        return x * 4;
}

unsigned
quarter(unsigned x)
{
        return x / 4;
}

unsigned
quarter_rest(unsigned x)
{
        return x % 4;
}
