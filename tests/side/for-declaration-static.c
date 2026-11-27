/* A storage class in a for's declaration is rcc's error, by C17's
   rule that it declare objects of storage class auto or register only,
   6.8.5p3, said at the keyword. */
int
main(void)
{
        for (static int i = 0; i < 3;)
                ;
        return 0;
}
