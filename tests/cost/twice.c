/* Chapter 1, no merging; chapter 2, -disable-machine-cse.  The same
   expression written twice is computed twice.  The listing of twice()
   must show two loads of a, two of b, two imul and the add of the two
   products.  Each read is a load of its own, so machine CSE would see
   two different products there anyway.  In twice_constant(), 12 * 34
   is an integer constant expression, folded in the parser to 408: the
   listing must show two adds of the immediate 0x198, one per product
   written, and no mov or imul, so nothing is left for machine CSE to
   drop and the flag has no witness here. */
int twice(int a, int b)
{
    return a * b + a * b;
}

int twice_constant(int a)
{
    return a + 12 * 34 + 12 * 34;
}
