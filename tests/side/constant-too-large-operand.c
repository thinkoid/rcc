/* A right operand that is a constant past 64 bits, which no type in
   its list holds, 6.4.4.2p7: said at the constant. */
int
main(void)
{
        int a = 1;
        return a ^ 99999999999999999999;
}
