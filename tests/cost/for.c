/* Chapter 3, for: the initializer once, the test at the top, the
   body, the step, the jump back.  The listing must show the blocks in
   that order, the counter and the sum each in its own slot. */
int sum(int n)
{
    int s = 0;
    for (int i = 0; i < n; i = i + 1)
        s = s + i;
    return s;
}
