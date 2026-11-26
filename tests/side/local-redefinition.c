/* A local declared twice in one scope, 6.7.1p4: the error is said at
   the second name, before its initializer is read, so y, declared
   nowhere, draws no error of its own. */
int
main(void)
{
        int x = 1;
        int x = y;
        return x;
}
