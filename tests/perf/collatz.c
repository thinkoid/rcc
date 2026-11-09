/* The performance leg, collatz: the step counts of the Collatz
   sequence.  Each repetition takes a start between 1 and 65536 from a
   linear congruential generator and, for each of the 1000 values from
   there, counts the steps to 1, halving an even value and taking
   three times an odd one plus one, and adds the count to a total.  The
   exit status is the total, folded to a byte.  This is the plain
   version: every variable is a slot local, and the parity test is a
   remainder and the halving a division, by 2 both.  The value is a
   signed long here and in collatz-tuned.c: x / 2 and x >> 1 differ
   for a negative value, so the tuned version's shift rests on what
   the programmer knows.  collatz-tuned.c is the same program written
   for a literal compiler. */
#define COUNT 1000
#define REPS 3000

int main(void)
{
    unsigned seed = 1;
    long total = 0;

    for (int rep = 0; rep < REPS; ++rep) {
        seed = seed * 1103515245 + 12345;

        long start = 1 + (seed >> 16);

        for (long v = start; v < start + COUNT; ++v) {
            long x = v;
            int steps = 0;

            while (x != 1) {
                if (x % 2 == 0)
                    x = x / 2;
                else
                    x = 3 * x + 1;
                ++steps;
            }
            total += steps;
        }
    }

    return total & 255;
}
