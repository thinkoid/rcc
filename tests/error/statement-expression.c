/* A statement expression, GNU C's ({ ... }), a block whose last
   expression is its value: rcc does not compile it yet. */
int
main(void)
{
        int x = ({
                int t = 1;
                t + 1;
        });

        return x;
}
