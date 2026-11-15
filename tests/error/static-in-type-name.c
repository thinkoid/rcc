/* static in a cast's type name: a type name has a specifier-qualifier
   list, and no storage class, 6.7.8p1. */
int
main(void)
{
        return (static int) 1;
}
