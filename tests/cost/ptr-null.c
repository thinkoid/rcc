/* backend.md 7.1 and aarch64.md 7.1, a pointer as a condition, and 6.3
   of both, p == 0 as a value.

   On x86-64 the listing must show test() comparing the pointer with
   cmpq $0, %rax once and branching with je, then each return's movl and
   jmp to the epilogue label, the last one included; and equal()
   comparing with cmpq $0, %rax and sete, widened with movzbl %al, %eax.

   On aarch64 the listing must show test() testing the pointer with
   cbz x0 once, then each return's mov and b to the epilogue label, the
   last one included; and equal() comparing with cmp x0, #0 and
   cset w0, eq. */
int test(int *p)
{
    if (p)
        return 1;
    return 0;
}

int equal(int *p)
{
    return p == 0;
}
