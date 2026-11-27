/* A constant no type holds as the argument of an unsigned long: the
   error is the constant's, said before any conversion to the
   parameter's type. */
int g(unsigned long a);

int
f(void)
{
        return g(9223372036854775808);
}
