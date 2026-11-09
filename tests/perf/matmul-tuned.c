/* The performance leg, matmul, tuned: matmul.c written for a literal
   compiler, with the strength reduction done by hand.  The row bases
   of a and c and the end of a's row are computed once per row, and
   the end of b's first row once per repetition, not at each
   access.  The inner loop walks a pointer along a's row and
   another down b's column, stepping by n, so it multiplies only the
   two elements; the sum is stored to c once, so c needs no clearing.
   The machine keeps five register variables, so register is on the
   five the inner loop uses, n, aend, pa, pb and s; the row and column
   walkers of the outer loops, arow, pc and bcol, and bend and i take
   slots.  Every loop ends on !=.  The fill, the change to a and the
   checksum are as in matmul.c. */
#define SIZE 9216
#define REPS 1200

int main(void)
{
    int a[SIZE];
    int b[SIZE];
    int c[SIZE];
    register int n = 96;
    unsigned seed = 1;
    unsigned sum = 0;

    for (int i = 0; i < SIZE; ++i) {
        seed = seed * 1103515245 + 12345;
        a[i] = (seed >> 16) % 256;
        seed = seed * 1103515245 + 12345;
        b[i] = (seed >> 16) % 256;
    }

    for (int rep = 0; rep < REPS; ++rep) {
        seed = seed * 1103515245 + 12345;
        a[(seed >> 16) % SIZE] = (seed >> 8) % 256;

        int *bend = b + n;

        for (int i = 0; i != n; ++i) {
            int *arow = a + i * n;
            register int *aend = arow + n;
            int *pc = c + i * n;
            int *bcol = b;

            while (bcol != bend) {
                register int *pa = arow;
                register int *pb = bcol;
                register int s = 0;

                while (pa != aend) {
                    s += *pa * *pb;
                    ++pa;
                    pb += n;
                }
                *pc = s;
                ++pc;
                ++bcol;
            }
        }

        for (int i = 0; i < SIZE; ++i)
            sum = sum * 31 + c[i];
    }

    return sum & 255;
}
