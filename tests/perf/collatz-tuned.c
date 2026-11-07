/* The performance leg, collatz, tuned: collatz.c written for a
   literal compiler.  The value is never negative and the divisor is
   2, so the parity test is & 1 and the halving >> 1: an and and a
   shift where collatz.c divides twice per step.  The compiler cannot
   know the sign; the programmer does.  The end of the range is
   computed once, the range loop ends on !=, and every variable of the
   two loops is a register variable, so none goes to its slot and back
   at each step. */
#define COUNT 1000
#define REPS 3000

int main(void)
{
    unsigned seed = 1;
    register long total = 0;

    for (register int rep = 0; rep != REPS; ++rep) {
        seed = seed * 1103515245 + 12345;

        register long start = 1 + (seed >> 16);
        register long end = start + COUNT;

        for (register long v = start; v != end; ++v) {
            register long x = v;
            register int steps = 0;

            while (x != 1) {
                if (x & 1)
                    x = 3 * x + 1;
                else
                    x = x >> 1;
                ++steps;
            }
            total += steps;
        }
    }

    return total & 255;
}
