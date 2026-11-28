/* Two pointers to distinct types under a relational operator are an
   error at the operator. */
int
f(void)
{
        int x;
        int *p = &x;
        int **pp = &p;
        return p < pp;
}
