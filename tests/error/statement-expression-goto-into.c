/* A goto from outside a statement expression to a label inside it,
   which would jump into the expression: GNU C takes no such jump, and
   clang refuses it.  The goto inside, back to the label, is taken. */
int
main(void)
{
        int n = 3;

        goto again;

        return ({
                int r = 0;
        again:
                if (n-- > 0) {
                        r++;
                        goto again;
                }
                r;
        });
}
