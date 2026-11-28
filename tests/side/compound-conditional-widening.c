/* The right operand of a compound assignment converted to the wider
   type of the operation: a ?: there is widened, and rcc hands the
   widening to its arms where one widens free, here both loads, movslq
   -8(%rbp), %rax on each path, then the addition at 64 bits. */
int
f(int a, int b, int c)
{
        long x = 5;
        x += a ? b : c;
        return 0;
}
