/* A statement expression as the initializer of a block's static
   object, which clang folds to its value, GNU C's extension: rcc does
   not fold one yet. */
int
main(void)
{
        static int k = ({ 1; });

        return k;
}
