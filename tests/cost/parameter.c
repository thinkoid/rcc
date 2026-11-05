/* Chapter 3, a parameter: stored from its argument register to its
   slot at entry, loaded from the slot at each read.  The listing must
   show edi and esi stored to two slots, and a read twice loaded
   twice. */
int parameter(int a, int b)
{
    return a - b + a;
}
