/* An array size that is not an integer, 6.7.7.3p1: a string literal
   decays to a pointer. */
int
main(void)
{
        int a["four"];

        return 0;
}
