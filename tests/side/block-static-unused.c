/* A block's static that nothing reads is written all the same. */
int
f(void)
{
        static int s = 9;
        return 0;
}
