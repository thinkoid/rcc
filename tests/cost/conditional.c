/* Chapter 3, a ? b : c: branches and conditional jumps.  The listing
   must show a compared with 0, a je to the block that loads c, the
   block that loads b and jumps past it, and the two joining at the
   ret. */
int conditional(int a, int b, int c)
{
    return a ? b : c;
}
