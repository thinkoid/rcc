/* backend.md 6.3 and aarch64.md 6.3, &x: the address of x's slot.
   Every local has its slot, so taking an address changes nothing about
   where x lives.

   On x86-64 the listing must show a stored to its slot and copied to
   x's at -8(%rbp), x's address taken with leaq -8(%rbp), %rax and
   stored to p's slot with movq, p loaded back, and *p read through it
   with movl (%rax), %eax.

   On aarch64 the listing must show a stored to its slot and copied to
   x's at [x29, #20], x's address taken with add x0, x29, #20 and
   stored to p's slot with str x0, p loaded back with ldr x0, and *p
   read through it with ldr w0, [x0]. */
int address(int a)
{
    int x = a;
    int *p = &x;

    return *p;
}
