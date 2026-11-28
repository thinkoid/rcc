/* A pointer and 0 under a relational operator is an error at the
   operator. */
int
f(void)
{
        int x;
        int *p = &x;
        return p < 0;
}
