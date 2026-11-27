/* A for whose condition has no ';', said at the token found in its
   place. */
int
main(void)
{
        int a = 1;
        for (a = 0; a < 1)
                ;
        return 0;
}
