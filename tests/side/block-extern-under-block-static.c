/* A block's extern under a block's static of its name names the file's
   object: the walk stops at the static, which has no linkage. */
int g = 1;
int
f(void)
{
        static int g = 2;
        {
                extern int g;
                return g;
        }
}
