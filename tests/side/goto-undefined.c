/* A goto names a label of its function, 6.8.7.2p1, defined before it
   or after it, so a name no label defines is an error at the end of
   the function, said at the goto's name. */
int
main(void)
{
        goto L;
        return 0;
}
