/* Chapter 1, no reordering; chapter 2, -disable-block-placement.
   Blocks come out in the written order.  The then-block returns from
   inside the loop, so a layout pass takes it for the unlikely exit
   and moves it after the loop.  The listing must show the je past the
   then-block, the mov of 1 and the ret, and only then the increment
   and the jump back. */
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
