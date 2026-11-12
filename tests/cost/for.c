/* backend.md 7.2 and aarch64.md 7.2, for: the initializer once, the
   test at the top, the body, the step, the jump back.

   On x86-64 the listing must show the blocks in that order, the step
   under a label of its own, and the sum at -8(%rbp) and the counter
   at -12(%rbp), each loaded from its slot at every read and stored at
   every write.

   On aarch64 the listing must show the blocks in that order, with no
   alignment directive before the test, the step under a label of its
   own, and the sum at [x29, #20] and the counter at [x29, #24], each
   set with str wzr, loaded from its slot at every read and stored at
   every write. */
int sum(int n)
{
    int s = 0;
    for (int i = 0; i < n; i = i + 1)
        s = s + i;
    return s;
}
