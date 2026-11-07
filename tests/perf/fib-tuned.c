/* The performance leg, fib, tuned: fib.c written for a literal
   compiler.  The parameter and the two partial sums are register
   variables: the parameter stays in the register it arrived in and
   has no slot, and the first partial sum is kept in a register across
   the second call.  Nothing else in the program is worth tuning: the
   work is the calls.  main is as in fib.c. */
#define REPS 800

int fib(register int n)
{
    if (n < 2)
        return n;

    register int a = fib(n - 1);
    register int b = fib(n - 2);

    return a + b;
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
