/* Chapter 3, while: the test at the top of the loop, a conditional
   jump out, an unconditional jump back.  The listing must show the
   counter loaded, compared and stored in memory on every iteration,
   and the two jumps. */
int count(int n)
{
    int i = 0;
    while (i < n)
        i = i + 1;
    return i;
}
