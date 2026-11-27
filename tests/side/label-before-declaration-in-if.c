/* A label where a statement is wanted labels a statement, and a
   declaration is none, 6.8.3p1: the error is said at its first token. */
int
main(void)
{
        int a = 1;
        if (a)
        L:
                int d;
        return 0;
}
