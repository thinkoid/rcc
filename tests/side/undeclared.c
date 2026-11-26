/* A name used after the block that declared it, where no declaration
   of it is in scope, 6.2.1p4: the error is said at the use. */
int
main(void)
{
        {
                int y = 1;
        }
        return y;
}
