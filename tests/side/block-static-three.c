/* Three static objects of one name in nested blocks: f.s, f.s.1 and
   f.s.2, the innermost read. */
int
f(void)
{
        static int s;
        {
                static int s = 1;
                {
                        static int s = 2;
                        return s;
                }
        }
}
