/* The performance leg, strings: length and copy over local char
   arrays.  The source array is filled once from a linear congruential
   generator with lower-case letters and, about one byte in 32, a
   terminating zero, so it holds a run of strings end to end; its last
   byte is always a zero.  Each repetition changes one byte to a letter
   or, one time in 32, to a zero, then takes each string in turn,
   measures it with length, copies it with copy, and folds the length
   and the middle byte of the copy into a checksum.  The exit status
   is the checksum, folded to a byte.  This is the plain version:
   every variable is a slot local, and length and copy index the
   strings by subscript.  strings-tuned.c is the same program written
   for a literal compiler. */
#define SIZE 8192
#define REPS 66000

int length(char *s)
{
    int n = 0;

    while (s[n] != 0)
        ++n;
    return n;
}

void copy(char *d, char *s)
{
    int i = 0;

    while (s[i] != 0) {
        d[i] = s[i];
        ++i;
    }
    d[i] = 0;
}

int main(void)
{
    char src[SIZE];
    char dst[SIZE];
    unsigned seed = 1;
    unsigned sum = 0;

    for (int i = 0; i < SIZE; ++i) {
        seed = seed * 1103515245 + 12345;
        if ((seed >> 16) % 32 == 0)
            src[i] = 0;
        else
            src[i] = 'a' + (seed >> 16) % 26;
    }
    src[SIZE - 1] = 0;

    for (int rep = 0; rep < REPS; ++rep) {
        seed = seed * 1103515245 + 12345;
        if ((seed >> 8) % 32 == 0)
            src[(seed >> 16) % (SIZE - 1)] = 0;
        else
            src[(seed >> 16) % (SIZE - 1)] = 'a' + (seed >> 8) % 26;

        int i = 0;

        while (i < SIZE) {
            int n = length(src + i);

            copy(dst, src + i);
            sum = sum * 31 + n + dst[n / 2];
            i += n + 1;
        }
    }

    return sum & 255;
}
