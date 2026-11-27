/* An object tentative and then static initialized: static after a declaration of external linkage would give
   the name both linkages, 6.2.2p7; the error
   is at the later declaration's name. */
int x;
static int x = 3;

int
f(void)
{
        return 0;
}
