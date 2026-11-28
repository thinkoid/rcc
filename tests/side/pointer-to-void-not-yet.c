/* A pointer to void is not yet, at its *. */
int
f(void)
{
        int x;
        return (void *) &x == 0;
}
