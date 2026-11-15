/* A call through a pointer with fewer arguments than the pointed-to
   prototype has parameters, 6.5.3.3p2. */
int
call(int (*p)(int, int))
{
        return p(1);
}

int
main(void)
{
        return 0;
}
