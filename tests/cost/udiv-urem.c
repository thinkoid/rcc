/* backend.md 6.3 and aarch64.md 6.3, a / b and a % b, unsigned: the
   unsigned division, and the remainder as the machine forms it.

   On x86-64 the listing must show xorl %edx, %edx and divl %ecx in both
   functions, the quotient from eax and the remainder from edx: udiv()
   returning eax as it stands and urem() moving edx to eax with
   movl %edx, %eax.

   On aarch64, which has no remainder instruction, the listing must show
   udiv() as udiv w0, w0, w1, and urem() as udiv w2, w0, w1 and
   msub w0, w2, w1, w0, the quotient multiplied back and subtracted. */
unsigned udiv(unsigned a, unsigned b)
{
    return a / b;
}

unsigned urem(unsigned a, unsigned b)
{
    return a % b;
}
