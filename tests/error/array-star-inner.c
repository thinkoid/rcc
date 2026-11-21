/* '*' for the size of an array in a parameter's brackets other than
   those it is adjusted from: the parameter is a pointer to a variable
   length array of unspecified size, 6.7.7.3p4, which rcc does not
   have yet. */
void f(int a[3][*]);

int
main(void)
{
        return 0;
}
