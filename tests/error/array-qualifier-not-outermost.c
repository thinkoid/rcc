/* A qualifier in the brackets of an array no parameter is adjusted
   from: qualifiers and static stand only in a parameter's outermost
   array type derivation, 6.7.7.3p1. */
int f(int a[3][const 4]);

int
main(void)
{
        return 0;
}
