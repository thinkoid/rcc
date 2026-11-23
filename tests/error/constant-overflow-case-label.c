/* A case label whose sum overflows int: 6.6p4 makes a constant
   expression out of its type's range a constraint violation, so rcc
   warns, and the label is the wrapped value, the most negative int. */
int
main(void)
{
        int n = -2147483647 - 1;

        switch (n) {
        case 2147483647 + 1: return 0;
        }

        return 1;
}
