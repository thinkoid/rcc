/* An array whose element is a variable length array, 6.7.7.3p4: rcc
   does not compile it yet. */
int
f(int n)
{
        int a[3][n];

        return 0;
}

int
main(void)
{
        return f(1);
}
