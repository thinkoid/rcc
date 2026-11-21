/* A label inside a statement expression, which GNU C takes with a goto
   inside it and refuses with a goto from outside: rcc does not take it
   yet. */
int
main(void)
{
        int n = 3;

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
