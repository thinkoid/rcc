/* backend.md chapter 4, an uninitialized register local: x has no
   initializer and is ebx, which nothing writes, so both reads see
   whatever rbx held on entry (G5).  The listing must show rbx pushed,
   no slot and no load, both compares cmpl $10, %ebx against the
   register itself, the inverse jumps jge and jl, and each return
   jumping to the epilogue label, the last one included. */
int which(void)
{
    register int x;
    if (x < 10)
        return 1;
    if (x >= 10)
        return 2;
    return 3;
}
