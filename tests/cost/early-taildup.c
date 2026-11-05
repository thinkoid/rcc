/* Chapter 1, no copying; chapter 2, -disable-early-taildup and
   -disable-tail-duplicate.  The block where the two arms of the if
   join holds only the jump back to the loop test, and it stays one
   block.  The listing must show the then-arm jumping to the join, the
   else-arm falling into it, and the join's one jmp back to the test;
   tail duplication would copy that jmp into the then-arm. */
int join(int a, int n)
{
    int i = 0;
    while (i < n)
        if (a)
            i = i + 1;
        else
            i = i + 2;
    return i;
}
