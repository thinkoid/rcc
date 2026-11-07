/* Chapter 3, a register parameter: the argument itself, no slot.  The
   listing must show no stack operand, no store at entry and no load:
   a copied to eax, since the two-operand sub would destroy it and a is
   read again, then the arithmetic on esi and edi themselves, where
   parameter.c stores both and loads three times. */
int parameter(register int a, register int b)
{
    return a - b + a;
}
