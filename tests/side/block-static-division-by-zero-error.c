/* A block's static initialized by a division by 0. */
int
f(void)
{
        static int s = 1 / 0;
        return s;
}
