/* Chapter 3, if: the condition compared with 0, a conditional jump
   past the then-block, an unconditional jump past the else-block.
   The listing must show the cmp and setl of a < b, its value compared
   with 0, the je, the then-block's store, the jmp over the
   else-block, and the else-block's store. */
int choose(int a, int b)
{
    int x;
    if (a < b)
        x = a;
    else
        x = b;
    return x;
}
