/* A declaration as a while's body: a declaration is a block item and
   no statement, 6.8.3p1, said at its first token. */
int
main(void)
{
        int a = 1;
        while (a)
                int b;
        return 0;
}
