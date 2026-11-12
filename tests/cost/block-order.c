/* cost.md chapter 1, no reordering: blocks come out in the written
   order.  The then-block returns from inside the loop.

   On x86-64 the listing must show the test i < n at the top and its jge
   to the block after the loop, the compare of i with a and its jne past
   the then-block, the then-block's movl $1, %eax and jmp to the
   epilogue label, and only then the increment and the jmp back to the
   test.

   On aarch64 the listing must show the test i < n at the top and its
   b.ge to the block after the loop, the compare of i with a and its
   b.ne past the then-block, the then-block's mov w0, #1 and b to the
   epilogue label, and only then the increment and the b back to the
   test. */
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
