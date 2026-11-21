/* A compound assignment to a _Bool whose operand has a side effect:
   rcc builds b op= e as b = (_Bool) (b op (e)), 6.5.17.3p4, which
   names b twice, and the operand is evaluated once, p4, so the one
   with a side effect, *p++, is not yet. */
int
main(void)
{
        _Bool flags[2] = { 0, 1 };
        _Bool *p = flags;

        *p++ |= 1;
        return flags[0];
}
