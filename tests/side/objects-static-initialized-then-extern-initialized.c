/* An object static initialized and then extern initialized: a second initializer is a second definition, 6.9.1p5; the error
   is at the later declaration's name. */
static int x = 3;
extern int x = 2;

int
f(void)
{
        return 0;
}
