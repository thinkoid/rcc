/* Chapter 3, int * to void * and back: pointer to pointer is no
   instruction.  The listing must show p stored to v's slot, v loaded
   and stored to q's slot, and no instruction between a load and the
   next store. */
int *round(int *p)
{
    void *v = p;
    int *q = v;

    return q;
}
