/* A step on a _Bool whose operand has a side effect: ++E on a _Bool is
   E += 1, 6.5.4.1p2, which rcc builds as E = (_Bool) (E + 1), and E is
   evaluated once, 6.5.17.3p4, so the one with a side effect, *p++, is
   not yet. */
int
main(void)
{
        _Bool flags[2] = { 0, 1 };
        _Bool *p = flags;

        ++*p++;
        return flags[0];
}
