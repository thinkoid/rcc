/* backend.md chapter 4, a read of a register variable is the register
   as an operand: x == x on a register variable.  The listing must
   show rbx pushed in the prologue and popped in the epilogue, n, an
   ordinary parameter, stored to -12(%rbp) below the push, loaded and
   moved into ebx, then cmpl %ebx, %ebx with the register as both
   operands, sete and movzbl: a compare, not its answer. */
int same(int n)
{
    register int x = n;
    return x == x;
}
