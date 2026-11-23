/* A statement expression as the initializer of a block's static
   object, which clang folds to its value: it is no operand of a
   constant expression, 6.6p10. */
int
main(void)
{
        static int k = ({ 1; });

        return k;
}
