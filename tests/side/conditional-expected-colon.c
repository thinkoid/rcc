/* A conditional's second operand is followed by its ':', 6.5.16p1,
   said missing at the token found in its place. */

int
main(void)
{
        int a;
        int b;
        int c;
        return a ? b;
}
