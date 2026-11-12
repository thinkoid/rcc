/* G13: left to right, across a branch. In x + (c ? x++ : 2) the left
   operand reads x before an arm writes it. Before the test of c, c1
   stores x's register, all 8 bytes, to a spill slot, since an arm
   writes x, and it reloads the slot after the arms meet.

   On x86-64 x is ebx: movq %rbx, -24(%rbp) before the test, and
   movq -24(%rbp), %rcx after the arms.

   On aarch64 x is w19: str x19, [x29, #32] before the test, and
   ldr x1, [x29, #32] after the arms.

   pick(5, 1) returns the old x plus the old x, 10, and pick(5, 0)
   returns 7: exit status 10 * 16 + 7 = 167. The expression is
   undefined in ISO C (6.5.1p2) when c is not zero, since x is read and
   written unsequenced. clang -O2 writes pick as movl $2, %eax,
   cmovnel %edi, %eax and addl %edi, %eax: the old x in both places,
   167. */
int pick(register int x, int c)
{
    return x + (c ? x++ : 2);
}

int main(void)
{
    return pick(5, 1) * 16 + pick(5, 0);
}
