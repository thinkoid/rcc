/* cost.md chapter 1, no reordering: blocks come out in the written
   order.  The then-block returns from inside the loop.  The listing
   must show the test i < n at the top and its jge to the block after
   the loop, the compare of i with a and its jne past the then-block,
   the then-block's movl $1, %eax and jmp to the epilogue label, and
   only then the increment and the jmp back to the test. */
int block_order(int a, int n)
{
    int i = 0;
    while (i < n) {
        if (i == a)
            return 1;
        i = i + 1;
    }
    return 0;
}
