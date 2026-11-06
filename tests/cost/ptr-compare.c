/* Chapter 3, p < q: icmp ult on ptr, the addresses compared unsigned
   (G7).  The listing must show one cmp of the two 64-bit addresses
   and setb, the unsigned condition, not setl, widened to int with
   and $1 and movzbl. */
int less(int *p, int *q)
{
    return p < q;
}
