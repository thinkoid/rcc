/* A static assertion whose sum overflows int: 6.6p4 makes a constant
   expression out of its type's range a constraint violation, so rcc
   warns, and the assertion tests the wrapped value, which is
   negative. */
_Static_assert(2147483647 + 1 < 0, "the sum wraps");

int
main(void)
{
        return 0;
}
