/* A declaration as a do's body: a declaration is a block item and no
   statement, 6.8.3p1, said at its first token. */
int
main(void)
{
        do
                int b;
        while (1);
        return 0;
}
