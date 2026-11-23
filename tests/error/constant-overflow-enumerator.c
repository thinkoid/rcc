/* An enumerator whose product overflows int: 6.6p4 makes a constant
   expression out of its type's range a constraint violation, so rcc
   warns, and the enumerator is the wrapped value, 0. */
enum { wide = 65536 * 65536 };

int
main(void)
{
        return wide;
}
