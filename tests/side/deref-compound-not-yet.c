/* A compound assignment through a pointer is not yet, at its operator. */
int
f(int *p)
{
        *p += 1;
        return 0;
}
