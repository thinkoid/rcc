/* An index of an int under a cast to unsigned is not yet, at the cast. */
int
f(int *p, int i)
{
        return p[(unsigned) i];
}
