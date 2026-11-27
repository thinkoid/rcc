/* A name a for's init declares is in scope for the loop alone,
   6.8.6.4p1: its use after the loop is an error at the use. */
int
main(void)
{
        for (int i = 0; i < 3; i = i + 1)
                ;
        return i;
}
