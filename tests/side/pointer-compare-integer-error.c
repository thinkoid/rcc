/* A pointer compared with an integer other than a null pointer
   constant is an error at the operator. */
int
f(void)
{
        int x;
        int *p = &x;
        return p == 1;
}
