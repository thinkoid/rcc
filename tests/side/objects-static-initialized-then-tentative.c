/* An object static initialized and then tentative: a declaration without a storage class after a static one
   would give the name both linkages, 6.2.2p7; the error
   is at the later declaration's name. */
static int x = 3;
int x;

int
f(void)
{
        return 0;
}
