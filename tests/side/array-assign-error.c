/* An array is no modifiable lvalue, 6.3.2.1p1: an assignment to it is
   an error at the '='. */
int
f(int *p)
{
        int a[3];
        a = p;
        return 0;
}
