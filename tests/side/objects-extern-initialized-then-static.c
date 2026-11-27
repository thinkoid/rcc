/* An object extern initialized and then static: static after a declaration of external linkage would give
   the name both linkages, 6.2.2p7; the error
   is at the later declaration's name. */
extern int x = 2;
static int x;

int
f(void)
{
        return 0;
}
