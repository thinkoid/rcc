/* backend.md chapter 4 and aarch64.md chapter 4, a read of a register
   variable is the register as an operand: x == x on a register variable
   is a compare, not its answer.

   On x86-64 the listing must show rbx pushed in the prologue and popped
   in the epilogue, n, an ordinary parameter, stored to -12(%rbp) below
   the push, loaded and moved into ebx, then cmpl %ebx, %ebx with the
   register as both operands, sete and movzbl.

   On aarch64 the listing must show x19 stored to the save area at
   [x29, #16] in the prologue and loaded back in the epilogue, n, an
   ordinary parameter, stored to [x29, #24] above it, loaded and moved
   into w19, then cmp w19, w19 with the register as both operands, and
   cset w0, eq into a fresh register. */
int same(int n)
{
    register int x = n;
    return x == x;
}
