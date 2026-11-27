/* An object extern initialized and then extern initialized: a second initializer is a second definition, 6.9.1p5; the error
   is at the later declaration's name. */
extern int x = 2;
extern int x = 2;

int
f(void)
{
        return 0;
}
