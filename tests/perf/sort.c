/* The performance leg, sort: insertion sort of a local int array.
   Each repetition fills the array with 2048 values from a linear
   congruential generator, sorts it in place, shifting each larger
   element up one place until the key's place is found, and folds the
   sorted array into a checksum that depends on the order.  The exit
   status is the checksum, folded to a byte.  This is the plain
   version: every variable is a slot local, and the shift indexes the
   array by subscript.  sort-tuned.c is the same program written for a
   literal compiler. */
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

        for (int i = 1; i < N; ++i) {
            int key = a[i];
            int j = i - 1;

            while (j >= 0 && a[j] > key) {
                a[j + 1] = a[j];
                --j;
            }
            a[j + 1] = key;
        }

        for (int i = 0; i < N; ++i)
            sum = sum * 31 + a[i];
    }

    return sum & 255;
}
