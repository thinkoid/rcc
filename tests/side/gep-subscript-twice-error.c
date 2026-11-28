/* An int subscripted again is an error at the second [. */
int
f(int *p, int i)
{
        return p[i][0];
}
