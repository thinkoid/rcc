/* A chain of comparisons is the grammar's association, 6.5.9p1,
   left-associative: a < b < c is (a < b) < c, the value 1 or 0 of the
   first compared with c, and a == b == c the same with ==.  The inner
   comparison's value is the outer one's left operand, held in its
   register, cmpl -12(%rbp), %eax after setl and movzbl, cmp w0, w1
   after cset.  A relational inside an equality is the tighter one:
   a < b == c < d is (a < b) == (c < d), the second comparison's value
   in a register of its own.

   less_chain() and equal_chain() are the two chains, mixed_chain()
   mixes the operators, and grouped() is a relational on each side of
   ==. */
int
less_chain(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return a < b < c;
}

int
equal_chain(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return a == b == c;
}

int
mixed_chain(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return a >= b != c <= a;
}

int
grouped(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        int d = 4;
        return a < b == c < d;
}
