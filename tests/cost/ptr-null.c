/* backend.md 7.1, a pointer as a condition, and 6.3, p == 0 as a
   value.  The listing must show test() comparing the pointer with
   cmpq $0, %rax once and branching with je, then each return's movl
   and jmp to the epilogue label, the last one included; and equal()
   comparing with cmpq $0, %rax and sete, widened with
   movzbl %al, %eax. */
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
