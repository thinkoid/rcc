/* A function defined in a block: a function definition is an external
   declaration, 6.9.1p1, and a block holds declarations and statements,
   6.8.3p1. */
int
main(void)
{
        int g(void)
        {
                return 0;
        }

        return g();
}
