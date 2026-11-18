/* An object declared extern in the block that declared a local of the
   name: the local has no linkage, so the extern declaration is of
   another entity, and a block declares a name of no linkage once,
   6.7.1p4. */
int count;

int
main(void)
{
        int count = 0;
        extern int count;

        return count;
}
