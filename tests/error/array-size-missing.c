/* A local array without a size: an object's type must be complete,
   6.7.1p8. */
int
main(void)
{
        int a[];

        return 0;
}
