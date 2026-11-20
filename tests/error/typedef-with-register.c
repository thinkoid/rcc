/* typedef and register in one declaration: at most one storage-class
   specifier. */
int
main(void)
{
        typedef register int word;

        return 0;
}
