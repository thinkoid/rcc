/* A parameter with the name of an enumeration constant the type of an
   earlier parameter declares: both are in the scope of the list,
   6.2.1p4, and the constant is no parameter. */
int f(enum { A } x, int A);

int
main(void)
{
        return 0;
}
