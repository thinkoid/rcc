/* The performance leg, matmul: the product of two 96 by 96 int
   matrices, each a flat local array in row order.  The matrices are
   filled once from a linear congruential generator; each repetition
   changes one element of a, clears c, computes c = a b by the
   textbook triple loop, c[i * n + j] += a[i * n + k] * b[k * n + j],
   and folds c into a checksum.  The exit status is the checksum,
   folded to a byte.  This is the plain version: every variable is a
   slot local, and every access computes its index from i, j, k and n.
   SIZE is n * n written out, since rcc takes an array size only as
   one integer constant.  matmul-tuned.c is the same program written
   for a literal compiler. */
#define SIZE 9216
#define REPS 1200

int main(void)
{
    int a[SIZE];
    int b[SIZE];
    int c[SIZE];
    int n = 96;
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

        for (int i = 0; i < n; ++i)
            for (int j = 0; j < n; ++j)
                c[i * n + j] = 0;

        for (int i = 0; i < n; ++i)
            for (int j = 0; j < n; ++j)
                for (int k = 0; k < n; ++k)
                    c[i * n + j] += a[i * n + k] * b[k * n + j];

        for (int i = 0; i < SIZE; ++i)
            sum = sum * 31 + c[i];
    }

    return sum & 255;
}
