/* Chapter 4, a compare of one value with itself: the constant 0
   tested as a condition and under !, so the operand and the zero it
   is compared with are one constant.  FastISel folds such a compare
   to its answer; the emitter freezes the zero, so the two operands
   are two values.  The listing must show a cmp in each function, the
   body of the if present, and no unconditional jump in place of the
   test. */
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
