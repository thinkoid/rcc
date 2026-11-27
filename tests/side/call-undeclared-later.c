/* A call of a function the unit defines after it: the definition does
   not declare the name in time, 6.2.1p7. */
int
f(void)
{
        return g();
}

int
g(void)
{
        return 1;
}
