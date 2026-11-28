/* &*p is no lvalue, and its assignment an error at the =. */
int
f(int *p)
{
        &*p = p;
        return 0;
}
