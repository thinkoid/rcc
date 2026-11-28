/* A sum compared with an integer is an error at the ==. */
int
f(int *p, int i)
{
        return p + i == i;
}
