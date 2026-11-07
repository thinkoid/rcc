/* Chapter 3, an uninitialized register local: x has no initializer,
   so it gets one slot nothing writes, read once at its declaration.
   The listing must show one load from the frame and both compares
   against that one value: no second load and no implicit definition,
   whose register the allocator need not keep between the reads. */
int which(void)
{
    register int x;
    if (x < 10)
        return 1;
    if (x >= 10)
        return 2;
    return 3;
}
