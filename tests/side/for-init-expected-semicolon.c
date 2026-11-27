/* A for whose init has no ';': the init is an expression statement,
   and its ';' is said missing at the token found in its place. */
int
main(void)
{
        int a = 1;
        for (a = 0 a < 1;)
                ;
        return 0;
}
