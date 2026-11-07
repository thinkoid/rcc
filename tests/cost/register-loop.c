/* Chapter 3, a register loop: the loop of for.c with register on the
   parameter, the sum and the counter.  None has a slot, so the
   listing must show no stack operand at all, the counter and the sum
   each in a register across the loop, where for.c moves the counter
   to and from its slot four times inside the loop. */
int sum(register int n)
{
    register int s = 0;
    for (register int i = 0; i < n; i = i + 1)
        s = s + i;
    return s;
}
