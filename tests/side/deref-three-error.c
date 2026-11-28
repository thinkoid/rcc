/* *** of the address of a pointer to int is an error at the first *. */
int
f(int x)
{
        int *p = &x;
        return ***&p;
}
