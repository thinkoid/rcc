/* A declaration as a for's body: a declaration is a block item and no
   statement, 6.8.3p1, said at its first token. */
int
main(void)
{
        for (;;)
                int b;
        return 0;
}
