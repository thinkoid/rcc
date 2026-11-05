/* Chapter 1, no folding: a + 0 is an add.  The listing must show the
   addition of the constant 0, an add $0 or the lea with displacement
   0 that the cost model accepts, and not a bare move. */
int add_zero(int a)
{
    return a + 0;
}
