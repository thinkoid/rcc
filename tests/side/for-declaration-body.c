/* A declaration as the body of a for that declares a name: the body is
   a statement, 6.8.3p1, so a declaration there is an error at its first
   token, the same name as the init's among them. */
int
main(void)
{
        for (int i = 0; i < 3; i = i + 1)
                int i;
        return 0;
}
