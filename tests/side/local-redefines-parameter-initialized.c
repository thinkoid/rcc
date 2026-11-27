/* A local of a parameter's name with an initializer: the error is at
   the name, before the initializer is read. */
int
g(int a)
{
        int a = 1;
        return a;
}
