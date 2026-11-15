/* _Complex with an integer type, which GNU C takes and C does not:
   _Complex is taken only among the specifiers of a floating type. */
int
main(void)
{
        _Complex int z;

        return 0;
}
