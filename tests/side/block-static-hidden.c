/* A local of an inner block hides a block's static. */
int
f(void)
{
        static int s = 1;
        {
                int s = 2;
                return s;
        }
}
