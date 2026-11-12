/* backend.md 6.3 and aarch64.md 6.3, p - q on char *: the size is 1, so
   the subtraction is the whole of it.

   On x86-64 the listing must show subq %rcx, %rax of the two addresses
   and no cqto and no idiv.

   On aarch64 the listing must show sub x0, x0, x1 of the two addresses
   and no sdiv. */
long diff(char *p, char *q)
{
    return p - q;
}
