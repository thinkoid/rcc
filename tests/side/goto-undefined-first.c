/* Of two gotos whose labels are never defined, the error names the
   first in the order they are written. */
int
main(void)
{
        goto M;
        goto L;
        return 0;
}
