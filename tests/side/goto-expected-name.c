/* A goto names its label by an identifier, 6.8.7.1p1: none is an error
   at the token found in its place. */
int
main(void)
{
        goto;
        return 0;
}
