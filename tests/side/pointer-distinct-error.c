/* Two pointers to distinct types compared are an error at the
   operator. */
int
f(void)
{
        int x;
        int *p = &x;
        int **pp = &p;
        return p == pp;
}
