/* An initializer that assigns to a | b: a value, no lvalue,
   6.5.17.1p2, said at its '='. */
int
main(void)
{
        int a = 1;
        int b = 2;
        int x = a | b = 3;
        return x;
}
