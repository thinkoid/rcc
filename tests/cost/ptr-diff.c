/* Chapter 3, p - q: ptrtoint, sub, sdiv by the element size.  The
   listing must show the sub of the two addresses, the size 4 in a
   register, cqto and the 64-bit idiv: a division, not a shift, for
   a size of 4 (G6). */
long diff(int *p, int *q)
{
    return p - q;
}
