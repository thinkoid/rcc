/* backend.md chapter 4 and aarch64.md chapter 4, a register parameter:
   moved from its argument register to its callee-saved register in the
   prologue, no slot.

   On x86-64 the listing must show a moved from edi into ebx and b from
   esi into r12d, no operand off rbp, a copied into eax, since subl
   writes its destination, then subl %r12d, %eax and addl %ebx, %eax
   with the registers themselves as operands, where parameter.c stores
   both and loads three times.

   On aarch64 the listing must show a moved from w0 into w19 and b from
   w1 into w20, no slot and no copy: sub w0, w19, w20 writes a fresh
   register with the variables' registers as its operands, then
   add w0, w0, w19. */
int parameter(register int a, register int b)
{
    return a - b + a;
}
