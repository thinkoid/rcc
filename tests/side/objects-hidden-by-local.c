/* A local hides an object of its name in its block, 6.2.1p4. */
int x = 1;
int
f(void)
{
        int x = 2;
        return x;
}
