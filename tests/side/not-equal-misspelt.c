/* ! is no binary operator: the return's expression ends at a, and the
   ! is where its ';' should be. */
int
main(void)
{
        int a = 1;
        int b = 2;
        return a !b;
}
