/* A static function used and never defined, 6.9.1p3: the error is at
   the use, at the end of the unit. */
static int g(void);

int
f(void)
{
        return g();
}
