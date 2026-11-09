/* backend.md 6.3, p - q: the sub, then a division by the element
   size.  The listing must show subq %rcx, %rax of the two addresses,
   the size in a register with movq $4, %rcx, since idiv takes no
   immediate, cqto and idivq %rcx: a division, not a shift, for a size
   of 4 (G6). */
long diff(int *p, int *q)
{
    return p - q;
}
