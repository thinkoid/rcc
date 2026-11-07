/* Chapter 4, a compare of one value with itself: x == x on a register
   variable, one SSA name at both reads.  FastISel folds such a compare
   to its answer; the emitter freezes the right operand, so the two
   operands are two values.  The listing must show a cmp and a sete,
   not a constant, and nothing for the freeze. */
int same(int n)
{
    register int x = n;
    return x == x;
}
