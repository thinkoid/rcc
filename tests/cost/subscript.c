/* backend.md 6.3 and aarch64.md 6.3, a[i]: one instruction, the
   machine's addressing form.

   On x86-64 the listing must show, for the store and again for the
   load, the array's address taken with leaq -24(%rbp), %rax and i
   loaded and widened with movslq %ecx, %rcx; then the store
   movl %edx, (%rax,%rcx,4) and the load movl (%rax,%rcx,4), %eax, and
   no imul or shl for the address.

   On aarch64 the listing must show, for the store and again for the
   load, the array's address taken with add x0, x29, #24 and i loaded
   with ldr w1 and not widened; then the store str w2, [x0, w1, sxtw #2]
   and the load ldr w0, [x0, w1, sxtw #2], the operand extending and
   scaling the index, and no other instruction for the address. */
int subscript(int i, int v)
{
    int a[4];

    a[i] = v;
    return a[i];
}
