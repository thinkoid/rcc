/* The performance leg, strings, tuned: strings.c written for a
   literal compiler.  length walks a pointer to the terminator and
   returns the difference of two char pointers, which is a subtraction
   with no division.  copy walks both pointers and tests the byte it
   has just stored, so each byte is loaded once.  main walks a pointer
   from string to string up to an end computed once, halves the length
   with a shift, since it is never negative, and keeps the walker, the
   length and the checksum in registers.  The pointers and counters of
   every hot loop are register variables, so none goes to its slot and
   back at each step.  The fill and the change to one byte are as in
   strings.c. */
#define SIZE 8192
#define REPS 66000

int length(register char *s)
{
    register char *p = s;

    while (*p != 0)
        ++p;
    return p - s;
}

void copy(register char *d, register char *s)
{
    while ((*d = *s) != 0) {
        ++d;
        ++s;
    }
}

int main(void)
{
    char src[SIZE];
    char dst[SIZE];
    unsigned seed = 1;
    register unsigned sum = 0;

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

        register char *p = src;
        register char *end = src + SIZE;

        while (p != end) {
            register int n = length(p);

            copy(dst, p);
            sum = sum * 31 + n + dst[n >> 1];
            p += n + 1;
        }
    }

    return sum & 255;
}
