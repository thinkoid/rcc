/* Chapter 1, no moving; chapter 2, -disable-machine-licm.  A
   loop-invariant expression is computed every iteration.  The listing
   of invariant() must show the loads of a and b, cltd and idiv inside
   the loop, between the test at the top and the jump back.  Its
   operands are loads, so machine LICM would leave it in place anyway.
   In invariant_constant(), 12 * 34 is an integer constant expression,
   folded in the parser to 408: the listing must show the add of the
   immediate 0x198 inside the loop and no mov or imul, so nothing is
   left for machine LICM to move and the flag has no witness here. */
int invariant(int a, int b, int n)
{
    int s = 0;
    for (int i = 0; i < n; i = i + 1)
        s = s + a / b;
    return s;
}

int invariant_constant(int n)
{
    int s = 0;
    for (int i = 0; i < n; i = i + 1)
        s = s + 12 * 34;
    return s;
}
