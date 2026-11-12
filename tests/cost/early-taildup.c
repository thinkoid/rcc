/* backend.md 7.2 and aarch64.md 7.2, an if and else inside a while: the
   arms join at the loop's jump back to the test, one block.

   On x86-64 the listing must show a compared with cmpl $0, %eax and a
   je to the else-arm, the then-arm adding 1 to i and jumping to the
   join, the else-arm adding 2 and falling into it, and the join's one
   jmp back to the test.

   On aarch64 the listing must show a tested with cbz w0 to the
   else-arm, the then-arm adding 1 to i and branching to the join, the
   else-arm adding 2 and falling into it, and the join's one b back to
   the test. */
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
