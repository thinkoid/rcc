/* A pointer constant as the condition of ?:, which rcc writes and
   rcc-v2 would fold, is not yet, at the ?. */
int
f(void)
{
        return (int *) 4 ? 1 : 2;
}
