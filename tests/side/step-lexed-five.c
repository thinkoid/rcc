/* a+++++b lexes as a ++ ++ + b, the longest token first, 6.4p4, so
   the second ++ has no modifiable lvalue. */
int g(int x);

int
main(void)
{
        int a = 1;
        int b = 2;
        return a++ ++ +b;
        return a;
}
