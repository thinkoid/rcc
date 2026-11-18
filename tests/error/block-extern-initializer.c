/* An object declared extern in a block with an initializer: a name of
   linkage declared in a block takes none, 6.7.11p6. */
int
main(void)
{
        extern int count = 1;

        return count;
}
