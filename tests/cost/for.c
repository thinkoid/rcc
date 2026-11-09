/* backend.md 7.2, for: the initializer once, the test at the top, the
   body, the step, the jump back.  The listing must show the blocks in
   that order, the step under a label of its own, and the sum at
   -8(%rbp) and the counter at -12(%rbp), each loaded from its slot at
   every read and stored at every write. */
int sum(int n)
{
    int s = 0;
    for (int i = 0; i < n; i = i + 1)
        s = s + i;
    return s;
}
