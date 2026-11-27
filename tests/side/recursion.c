/* A call of the function itself, or of one defined later, is the same
   call as any other.  The recursive steps of fib, fact and ack, whose
   base cases are conditions, beside them: fib's first result is a held
   value at the second call, spilled and read in its slot; fact's n is
   spilled at the call and multiplies in its slot; ack's outer first
   argument is stored before the inner call's two.  odd is declared
   before even, so the listing writes odd first, the order of first
   declaration. */
int odd(int n);

int
fib(int n)
{
        return fib(n - 1) + fib(n - 2);
}

int
fact(int n)
{
        return n * fact(n - 1);
}

int
ack(int m, int n)
{
        return ack(m - 1, ack(m, n - 1));
}

int
even(int n)
{
        return odd(n - 1);
}

int
odd(int n)
{
        return even(n - 1);
}
