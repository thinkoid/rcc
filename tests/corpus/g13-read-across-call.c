/* G13: left to right. In x + f(x++) the left operand reads x before
   the argument writes it. The write of x++ first copies the held read
   into an expression register, the argument is stored to its slot,
   and the copy is spilled before the call and reloaded after it.

   On x86-64 x is ebx: movl %ebx, %eax copies the read, x++ is
   movl %ebx, %ecx; addl $1, %ebx, and the copy is spilled by
   movq %rax, -24(%rbp) and reloaded by movq -24(%rbp), %rcx.

   On aarch64 x is w19: mov w0, w19 copies the read, x++ is
   mov w1, w19; add w19, w19, #1, and the copy is spilled by
   str x0, [x29, #32] and reloaded by ldr x1, [x29, #32].

   sum returns the old x, 5, plus f(5), 105: exit status 110. The
   expression is undefined in ISO C (6.5.1p2), since x is read and
   written unsequenced. clang -O2 writes sum as leal 100(,%rdi,2),
   %eax, twice x plus 100: 110. */
int f(int n)
{
    return n + 100;
}

int sum(register int x)
{
    return x + f(x++);
}

int main(void)
{
    return sum(5);
}
