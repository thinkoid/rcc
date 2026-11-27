/* Static objects of two names, each counted on its own: f.a, f.b,
   f.a.1 and f.b.1. */
int
f(void)
{
        static int a = 1;
        static int b;
        {
                static int a = 2;
                static int b = 3;
                return a + b;
        }
}
