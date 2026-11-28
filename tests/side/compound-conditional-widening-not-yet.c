/* The right operand of a compound assignment converted to the wider
   type of the operation: a ?: there is widened, which rcc writes into
   its arms, and which rcc-v2 says is not yet, at the '?'. */
int
f(int a, int b, int c)
{
        long x = 5;
        x += a ? b : c;
        return 0;
}
