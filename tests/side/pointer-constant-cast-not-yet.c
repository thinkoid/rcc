/* A pointer constant cast to an integer, no constant to rcc, is not
   yet, at the (. */
long
f(void)
{
        return (long) (int *) 4;
}
