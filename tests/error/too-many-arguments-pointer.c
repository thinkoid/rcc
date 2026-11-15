/* A call through a pointer with more arguments than the pointed-to
   prototype has parameters, 6.5.3.3p2. */
int
call(int (*p)(int))
{
        return p(1, 2);
}

int
main(void)
{
        return 0;
}
