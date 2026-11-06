/* Chapter 3, p + n; chapter 4, a standalone gep.  a + i stored to a
   pointer variable feeds no load, so the gep is selected on its own:
   the listing must show the index widened with cltq, the array's
   address taken with lea, the index scaled by shl $0x2 and added to
   it, and the sum stored to p's slot. */
int *gep(int i)
{
    int a[4];
    int *p;

    p = a + i;
    return p;
}
