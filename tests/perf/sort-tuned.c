/* The performance leg, sort, tuned: sort.c written for a literal
   compiler.  The shift walks two pointers down from the key's place,
   p the hole and q the element below it, so no access scales an
   index.  q steps by --q, an add of a constant, where p[-1] would
   negate the 1 at run time.  Each element compared is loaded once
   into v and stored from there.  The key, the index, the two walkers
   and v are register variables, so none of them goes to its slot and
   back at each step.  The outer loop ends on !=.  The fill and the
   checksum are as in sort.c. */
#define N 2048
#define REPS 1000

int main(void)
{
    int a[N];
    unsigned seed = 1;
    unsigned sum = 0;

    for (int rep = 0; rep < REPS; ++rep) {
        for (int i = 0; i < N; ++i) {
            seed = seed * 1103515245 + 12345;
            a[i] = seed >> 16;
        }

        for (register int i = 1; i != N; ++i) {
            register int key = a[i];
            register int *p = a + i;
            register int *q = p;
            register int v = 0;

            while (p != a && (v = *--q) > key) {
                *p = v;
                p = q;
            }
            *p = key;
        }

        for (int i = 0; i < N; ++i)
            sum = sum * 31 + a[i];
    }

    return sum & 255;
}
