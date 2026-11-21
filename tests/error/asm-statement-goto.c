/* asm goto of an empty template, which is not yet. */
int
main(void)
{
        asm goto("" : : : "memory" : out);

        return 0;
out:
        return 1;
}
