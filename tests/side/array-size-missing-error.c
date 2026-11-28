/* A local array's type is complete, 6.7.1p8: its size is written, or
   its initializer gives it, an error at its name. */
int
f(void)
{
        int a[];
        return 0;
}
