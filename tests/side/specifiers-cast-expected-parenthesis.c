/* A type name in a cast takes no name: an identifier after it is where
   the ')' is expected. */
int
f(void)
{
        int x = 0;
        return (long x) x;
}
