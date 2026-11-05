/* Chapter 1, no merging; chapter 2, -disable-machine-cse.  The same
   expression written twice is computed twice.  The listing of twice()
   must show two loads of a, two of b, two imul and the add of the two
   products.  Each read is a load of its own, so machine CSE would see
   two different products there anyway; the flag shows in
   twice_constant(), whose 12 * 34 has constant operands: the listing
   must show the mov of 34 and the imul by 12 twice, and machine CSE
   would drop the second pair. */
int twice(int a, int b)
{
    return a * b + a * b;
}

int twice_constant(int a)
{
    return a + 12 * 34 + 12 * 34;
}
