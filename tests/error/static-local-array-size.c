/* A block's static array without its size and without an initializer:
   an object of no linkage has a complete type by the end of its
   declarator, 6.7.1p8. */
int
main(void)
{
        static int table[];

        return 0;
}
