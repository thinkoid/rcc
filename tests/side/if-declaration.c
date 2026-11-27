/* A declaration as an if's then part: a declaration is a block item
   and no statement, 6.8.3p1, said at its first token. */
int
main(void)
{
        int a = 1;
        if (a)
                int b;
        return 0;
}
