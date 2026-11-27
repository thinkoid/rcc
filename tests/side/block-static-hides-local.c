/* A block's static hides a local of an enclosing block. */
int
f(void)
{
        int s = 1;
        {
                static int s = 2;
                return s;
        }
}
