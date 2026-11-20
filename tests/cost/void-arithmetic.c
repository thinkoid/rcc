/* cost.md chapter 1, arithmetic on a pointer to void: GNU C takes it
   as on a char *, a step of a byte, and rcc takes it so, with a warning
   (shape.md chapter 5).  forward() adds an index, distance() subtracts
   two pointers, and step() steps a pointer each way.

   On x86-64 the listing must be, byte for byte but for the .file line,
   the listing of this program with char * in place of each void *.

   On aarch64 the listing must be the same twin's listing, byte for
   byte but for the .file line. */
void *
forward(void *p, long n)
{
        return p + n;
}

long
distance(void *p, void *q)
{
        return p - q;
}

void *
step(void *p)
{
        ++p;
        p -= 2;
        p += 3;
        --p;
        return p++;
}
