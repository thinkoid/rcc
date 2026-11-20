/* A declaration as the body of an if: a declaration is a block item,
   not a statement. */
int
main(void)
{
        if (1)
                int x = 1;

        return 0;
}
