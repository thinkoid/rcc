/* ++ on an object declared const: the operand of a prefix ++ is a
   modifiable lvalue, 6.5.4.1p1, and a const one is none, 6.3.2.1p1. */
int
main(void)
{
        const int count = 1;

        ++count;
        return 0;
}
