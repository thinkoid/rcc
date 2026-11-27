/* A return with a value in a function whose result type is void,
   6.8.7.5p1: the error is at the return, after its value is read. */
void
g(void)
{
        int a = 1;
        return a + 1;
}
