/* The casts between pointers and integers: a pointer is an unsigned
   long to every row, so (long) p and (int) p write nothing, an int
   widens by movslq, an unsigned by movl, a long by nothing; a
   constant cast to a pointer is the pointer's constant, and a cast
   between pointer types writes nothing. */
long
to_long(void)
{
        int x = 1;
        int *p = &x;
        return (long) p;
}

int
to_int(void)
{
        int x = 1;
        int *p = &x;
        return (int) p == 0;
}

int
from_integers(int n, unsigned u, long l)
{
        int *p = (int *) n;
        int *q = (int *) u;
        int *r = (int *) l;
        return p == q && q == r;
}

int
from_constants(void)
{
        int *p = (int *) 4096;
        int *q = (int *) -1;
        int *r = (int *) 0;
        return p == q || q == r;
}

int
between_pointers(void)
{
        int x = 1;
        int *p = &x;
        int **pp = (int **) p;
        int **qq = &p;
        return (int **) p == qq && pp != qq;
}

unsigned long
unsigned_long(void)
{
        int x = 1;
        int *p = &x;
        return (unsigned long) p + (unsigned long) &x;
}
