/* A pointer initializing an integer is an error at the =. */
int
f(void)
{
        int y;
        int x = &y;
        return x;
}
