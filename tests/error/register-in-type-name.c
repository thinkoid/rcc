/* register in the type name of a cast: a type name takes no storage
   class, 6.7.8p1. */
int
main(void)
{
        return (register int) 1;
}
