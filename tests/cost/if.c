/* backend.md 7.2 and aarch64.md 7.2, if, and 7.1 of both, jump code:
   the condition is a compare and a jump, with no value.

   On x86-64 the listing must show a < b as cmpl %ecx, %eax and a jge
   past the then-block, no setl, the then-block's store of a to x's slot
   and its jmp over the else-block, and the else-block's store of b.

   On aarch64 the listing must show a < b as cmp w0, w1 and a b.ge past
   the then-block, no cset, the then-block's store of a to x's slot and
   its b over the else-block, and the else-block's store of b. */
int choose(int a, int b)
{
    int x;
    if (a < b)
        x = a;
    else
        x = b;
    return x;
}
