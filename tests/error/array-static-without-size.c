/* static in a parameter's brackets without a size: static takes one,
   6.7.7.3p3. */
int f(int a[static]);

int
main(void)
{
        return 0;
}
