/* backend.md 6.3 and aarch64.md 6.3, (T) a between pointer types: no
   instruction.

   On x86-64 the listing must show p stored to its slot, loaded and
   stored to v's slot, v loaded and stored to q's slot, all with movq,
   and no instruction between a load and the next store.

   On aarch64 the listing must show p stored to its slot, loaded and
   stored to v's slot, v loaded and stored to q's slot, all with str and
   ldr of x0, and no instruction between a load and the next store. */
int *round(int *p)
{
    void *v = p;
    int *q = v;

    return q;
}
