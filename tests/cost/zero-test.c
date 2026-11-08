/* Chapter 4, a compare of one value with itself: the constant 0
   tested as a condition, so the operand and the zero it is compared
   with are one constant.  FastISel folds such a compare to its
   answer; the emitter freezes the zero, so the two operands are two
   values.  The listing of never() must show a cmp, the body of the if
   present, and no unconditional jump in place of the test.  In
   not_zero(), !0 is an integer constant expression, folded in the
   parser to 1: the listing must show the mov of 1 and no cmp. */
int never(void)
{
    if (0)
        return 1;
    return 2;
}

int not_zero(void)
{
    return !0;
}
