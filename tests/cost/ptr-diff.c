/* backend.md 6.3 and aarch64.md 6.3, p - q: the subtraction, then a
   division by the element size, not a shift, for a size of 4 (G6).

   On x86-64 the listing must show subq %rcx, %rax of the two addresses,
   the size in a register with movq $4, %rcx, since idiv takes no
   immediate, cqto and idivq %rcx. */
long diff(int *p, int *q)
{
    return p - q;
}
