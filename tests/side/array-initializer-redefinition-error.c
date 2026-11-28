/* The name is declared before its initializer is read, so a second
   declaration is the error first. */
int
f(void)
{
        int a;
        int a[3] = 5;
        return 0;
}
