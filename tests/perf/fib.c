/* The performance leg, fib: Fibonacci by the doubly recursive
   definition.  Each repetition takes an argument between 24 and 27
   from a linear congruential generator and adds fib of it to a sum.
   The exit status is the sum, folded to a byte.  This is the plain
   version: the parameter is stored to its slot at entry, and each
   partial result goes to a temporary across the second call.
   fib-tuned.c is the same program written for a literal compiler. */
#define REPS 800

int fib(int n)
{
    if (n < 2)
        return n;
    return fib(n - 1) + fib(n - 2);
}

int main(void)
{
    unsigned seed = 1;
    int sum = 0;

    for (int rep = 0; rep < REPS; ++rep) {
        seed = seed * 1103515245 + 12345;
        sum += fib(24 + (seed >> 16) % 4);
    }

    return sum & 255;
}
