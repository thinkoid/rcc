/* An integer compared with a pointer is an error at the operator. */
int
f(int x)
{
        int *p = &x;
        return x == p;
}
