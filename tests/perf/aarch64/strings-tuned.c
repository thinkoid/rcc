/* The performance leg, strings, tuned for aarch64: strings.c written
   for a literal compiler.  On this target a load or a store through a
   register pointer steps the pointer in the same instruction, so the
   loops of length and copy are written with *p++, where the shared
   strings-tuned.c steps each pointer in a statement of its own.
   length walks a pointer past the terminator and returns the
   difference of two char pointers less one, which is two subtractions
   with no division; its loop is the load that steps p, a test of the
   byte and the branch back.  copy loads through s and stores through
   d, each stepping its pointer, and tests the byte it has just stored,
   so each byte is loaded once; its loop is the load, the store, the
   test and the branch back.  main walks a pointer from string to
   string up to an end computed once, halves the length with a shift,
   since it is never negative, and keeps the walker, the length and the
   checksum in registers.  The pointers and counters of every hot loop
   are register variables, so none goes to its slot and back at each
   step.  The fill and the change to one byte are as in strings.c. */
#define SIZE 8192
#define REPS 66000

int
length(register char *s)
{
    register char *p = s;

    while (*p++)
        ;
    return p - s - 1;
}

void
copy(register char *d, register char *s)
{
    while (*d++ = *s++)
        ;
}

int
main(void)
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
