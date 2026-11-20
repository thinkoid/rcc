/* The alignment specifier _Alignas, which is not yet. */
int
main(void)
{
        _Alignas(16) int x = 0;

        return x;
}
