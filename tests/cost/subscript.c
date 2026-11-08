/* backend.md 6.3, a[i]: one instruction, the machine's addressing
   form.  The listing must show, for the store and again for the load,
   the array's address taken with leaq -24(%rbp), %rax and i loaded
   and widened with movslq %ecx, %rcx; then the store
   movl %edx, (%rax,%rcx,4) and the load movl (%rax,%rcx,4), %eax, and
   no imul or shl for the address. */
int subscript(int i, int v)
{
    int a[4];

    a[i] = v;
    return a[i];
}
