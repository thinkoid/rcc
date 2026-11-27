/* A for's declaration ends at its ';', which is said missing at the
   token found in its place. */
int
main(void)
{
        for (int i = 0 i < 3;)
                ;
        return 0;
}
