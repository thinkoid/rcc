/* The check of the gotos waits for the end of the function, so an
   error of the body after the goto is said first, and alone. */
int
main(void)
{
        int a = 1;
        goto L;
        a = 1;
        5 = a;
        return 0;
}
