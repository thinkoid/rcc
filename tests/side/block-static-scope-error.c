/* A block's static is a name of its block alone. */
int
f(void)
{
        {
                static int s = 1;
        }
        return s;
}
