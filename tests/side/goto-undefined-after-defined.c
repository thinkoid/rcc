/* The check of the gotos waits for the end of the function, so a goto
   to a label defined later is none, and the first goto whose label is
   never defined is the error. */
int
main(void)
{
        int a = 1;
        if (a)
                goto L;
        goto M;
L:
        return 0;
}
