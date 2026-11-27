/* A body that ends in an if (c) goto L; ends where the condition's jump
   falls through, so main writes its return of 0. */
int
main(void)
{
        int a = 0;
L:
        a = a + 1;
        if (a < 3)
                goto L;
}
