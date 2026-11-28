/* ++a++ is ++(a++), a postfix operator binding first, 6.5.3.1p1, so
   the prefix ++ has no modifiable lvalue, said at the first ++. */
int g(int x);

int
main(void)
{
        int a = 1;
        int b = 2;
        ++a++;
        return a;
}
