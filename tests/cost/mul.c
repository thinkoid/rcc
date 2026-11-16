/* backend.md 6.3 and aarch64.md 6.3, a * b: one multiply.

   On x86-64 the listing must show a loaded into eax and one
   imull -8(%rbp), %eax, b's slot the operand.

   On aarch64 the listing must show a loaded into w0 and b into w1, and
   one mul w0, w0, w1. */
int mul(int a, int b)
{
    return a * b;
}
