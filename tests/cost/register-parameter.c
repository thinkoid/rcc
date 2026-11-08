/* backend.md chapter 4, a register parameter: moved from its argument
   register to its callee-saved register in the prologue, no slot.
   The listing must show a moved from edi into ebx and b from esi into
   r12d, no operand off rbp, a copied into eax, since subl writes its
   destination, then subl %r12d, %eax and addl %ebx, %eax with the
   registers themselves as operands, where parameter.c stores both and
   loads three times. */
int parameter(register int a, register int b)
{
    return a - b + a;
}
