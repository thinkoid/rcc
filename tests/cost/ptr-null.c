/* Chapter 3, a pointer as a condition and p == 0: icmp against null.
   The listing must show test() comparing the pointer with
   cmp $0x0 once and branching with je, no second comparison, since
   the condition is the pointer's own compare and not an int tested
   again; and equal() comparing with cmp $0x0 and sete, widened to
   int. */
int test(int *p)
{
    if (p)
        return 1;
    return 0;
}

int equal(int *p)
{
    return p == 0;
}
