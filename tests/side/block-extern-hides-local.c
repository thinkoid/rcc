/* A block's extern hides a local of an enclosing block. */
int g = 2;
int
f(void)
{
        int g = 1;
        {
                extern int g;
                return g;
        }
}
