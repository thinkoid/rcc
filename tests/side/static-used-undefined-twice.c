/* A static function used twice and never defined: the error is at the
   first use only. */
static int g(int a);

int
f(void)
{
        g(1);
        return g(2);
}
