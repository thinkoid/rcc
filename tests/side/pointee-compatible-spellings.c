/* The spellings of one type make one pointer type, signed int * and
   int *, long int * and long *, and an enumerated type and its
   compatible integer type point to compatible types; a cast between any
   two pointer types writes nothing. */
enum e { A = 1, B = 2 };

int *
f(signed int *p)
{
        return p;
}

long *
g(long int *p)
{
        return p;
}

unsigned *
h(enum e *p)
{
        return p;
}

long long *
casts(long *p)
{
        return (long long *) (char *) (unsigned short *) p;
}
