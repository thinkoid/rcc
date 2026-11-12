/* backend.md 6.3 and aarch64.md 6.3, a / b and a % b, signed: the
   signed division, and the remainder as the machine forms it.

   On x86-64 the listing must show a loaded into eax and b into ecx,
   cltd and idivl %ecx in both functions, the quotient from eax and the
   remainder from edx: sdiv() returning eax as it stands and srem()
   moving edx to eax with movl %edx, %eax.

   On aarch64, which has no remainder instruction, the listing must show
   a loaded into w0 and b into w1, then sdiv() as sdiv w0, w0, w1, and
   srem() as sdiv w2, w0, w1, the quotient in a register of its own, and
   msub w0, w2, w1, w0, the quotient multiplied back and subtracted. */
int sdiv(int a, int b)
{
    return a / b;
}

int srem(int a, int b)
{
    return a % b;
}
