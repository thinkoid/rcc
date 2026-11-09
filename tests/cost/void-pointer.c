/* backend.md 6.3, (T) a between pointer types: no instruction.  The
   listing must show p stored to its slot, loaded and stored to v's
   slot, v loaded and stored to q's slot, all with movq, and no
   instruction between a load and the next store. */
int *round(int *p)
{
    void *v = p;
    int *q = v;

    return q;
}
