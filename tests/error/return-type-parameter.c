/* g is a function of a, returning a pointer to a function of b.  The
   body's block is the scope of the parameter list that makes g a
   function, 6.9.2p10, so a is in it and b, a parameter of the pointed-to
   function's prototype, is not. */
int (*g(int a))(int b)
{
        if (b)
                return 0;

        return 0;
}

int
main(void)
{
        return 0;
}
