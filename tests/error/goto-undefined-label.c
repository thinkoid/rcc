/* A goto whose label the function never defines, 6.8.7.2p1: an error
   at the function's end, at the first such goto; a label may follow its
   goto, as done does. */
int
main(void)
{
        goto done;
        goto missing;
        goto other;
done:
        return 0;
}
