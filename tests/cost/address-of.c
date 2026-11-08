/* backend.md 6.3, &x: the address of x's slot.  Every local has its
   slot, so taking an address changes nothing about where x lives.
   The listing must show a stored to its slot and copied to x's at
   -8(%rbp), x's address taken with leaq -8(%rbp), %rax and stored to
   p's slot with movq, p loaded back, and *p read through it with
   movl (%rax), %eax. */
int address(int a)
{
    int x = a;
    int *p = &x;

    return *p;
}
