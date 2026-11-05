/* Chapter 1, no moving; chapter 2, -disable-machine-licm.  A
   loop-invariant expression is computed every iteration.  The listing
   of invariant() must show the loads of a and b, cltd and idiv inside
   the loop, between the test at the top and the jump back.  Its
   operands are loads, so machine LICM would leave it in place anyway;
   the flag shows in invariant_constant(), whose 12 * 34 has constant
   operands: the listing must show the mov of 34 and the imul by 12
   inside the loop, and machine LICM would move the mov before it. */
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
