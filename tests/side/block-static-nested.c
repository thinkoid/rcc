/* Three nested blocks' statics of one name. */
int
f(void)
{
        static int n = 1;
        {
                static int n = 2;
                {
                        static int n;
                        return n;
                }
        }
}
