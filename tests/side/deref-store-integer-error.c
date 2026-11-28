/* An integer stored through a pointer to a pointer is an error at the
   =. */
int
f(int **pp, int x)
{
        *pp = x;
        return 0;
}
