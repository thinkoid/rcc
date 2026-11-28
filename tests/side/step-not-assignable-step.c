/* A step's value is no lvalue, so the second ++ of a++++ has no
   modifiable lvalue, said at that ++. */
int g(int x);

int
main(void)
{
        int a = 1;
        int b = 2;
        a++ ++;
        return a;
}
