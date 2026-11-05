/* Chapter 1, no branch to conditional move.  a < b ? a : b is
   branches.  The listing must show the cmp and setl of a < b, its
   value compared with 0, a je and the two arms, and no cmov.  The
   branches come from the emitter's phi and from FastISel; removing
   -disable-early-ifcvt changes nothing here. */
int select(int a, int b)
{
    return a < b ? a : b;
}
