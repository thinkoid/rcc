/* The performance leg, sieve: Eratosthenes on a local char array.
   Each repetition takes a limit between 16384 and 32767 from a linear
   congruential generator, marks every number below the limit, strikes
   the multiples of each number still marked from 2 on, and counts the
   numbers left marked, the primes below the limit.  The exit status
   is the sum of the counts, folded to a byte.  This is the plain
   version: every variable is a slot local, and both loops index the
   array by subscript.  sieve-tuned.c is the same program written for
   a literal compiler. */
#define SIZE 65536
#define REPS 8000

int main(void)
{
    char flag[SIZE];
    unsigned seed = 1;
    int total = 0;

    for (int rep = 0; rep < REPS; ++rep) {
        seed = seed * 1103515245 + 12345;

        int n = 16384 + (seed >> 16) % 16384;
        int count = 0;

        for (int i = 0; i < n; ++i)
            flag[i] = 1;

        for (int i = 2; i < n; ++i) {
            if (flag[i]) {
                ++count;
                for (int j = i + i; j < n; j += i)
                    flag[j] = 0;
            }
        }

        total += count;
    }

    return total & 255;
}
