/* '*' for the size of the array a parameter of a definition is
   adjusted from: a variable length array of unspecified size is no
   part of a parameter declaration in a function definition, 6.9.2p6. */
int
f(int n, int a[*])
{
        return a[0];
}

int
main(void)
{
        return 0;
}
