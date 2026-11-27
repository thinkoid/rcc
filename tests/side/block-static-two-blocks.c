/* Two blocks' static objects of one name in one function: f.s, then
   f.s.1 for the second, counted per name in the function. */
int
f(void)
{
        int r = 0;
        {
                static int s = 1;
                r = r + s;
        }
        {
                static int s = 2;
                r = r + s;
        }
        return r;
}
