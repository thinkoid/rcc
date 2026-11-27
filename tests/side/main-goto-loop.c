/* A body that ends in a goto ends where no path reaches its end, so
   main writes no return of 0. */
int
main(void)
{
L:
        goto L;
}
