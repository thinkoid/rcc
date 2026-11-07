/* The performance leg, sieve, tuned: sieve.c written for a literal
   compiler.  The marking loop and the inner strike loop walk a
   pointer to an end pointer computed once per repetition, so neither
   scales an index at each access.  The walker, the end, the limit,
   the outer counter and the count are register variables, so none of
   them goes to its slot and back at each step.  The marking loop ends
   on !=; the strike loop steps by i and may pass the end, so it ends
   on <.  The array is twice the largest limit, so a walker that has
   stepped past the end still points into it. */
#define SIZE 65536
#define REPS 8000

int main(void)
{
    char flag[SIZE];
    unsigned seed = 1;
    int total = 0;

    for (int rep = 0; rep < REPS; ++rep) {
        seed = seed * 1103515245 + 12345;

        register int n = 16384 + (seed >> 16) % 16384;
        register int count = 0;
        register char *end = flag + n;

        for (register char *p = flag; p != end; ++p)
            *p = 1;

        for (register int i = 2; i != n; ++i) {
            if (flag[i]) {
                ++count;
                for (register char *p = flag + i + i; p < end; p += i)
                    *p = 0;
            }
        }

        total += count;
    }

    return total & 255;
}
