/* A local of an inner block hides the object there alone: the read
   after the block is the object's again. */
int x = 1;
int
f(void)
{
        int y = x;
        {
                int x = 2;
                y = y + x;
        }
        return y + x;
}
