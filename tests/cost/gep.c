/* backend.md 6.3, p + n for an element of 4 bytes: the lea of the
   addressing form.  a + i is stored to a pointer variable and feeds
   no load.  The listing must show the array's address taken with
   leaq -20(%rbp), %rax, i loaded and widened with movslq %ecx, %rcx,
   the sum formed by leaq (%rax,%rcx,4), %rax, and the sum stored to
   p's slot with movq. */
int *gep(int i)
{
    int a[4];
    int *p;

    p = a + i;
    return p;
}
