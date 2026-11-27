/* An object static initialized and then static initialized: a second initializer is a second definition, 6.9.1p5; the error
   is at the later declaration's name. */
static int x = 3;
static int x = 3;

int
f(void)
{
        return 0;
}
