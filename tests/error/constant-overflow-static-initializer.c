/* The initializer of an object of static storage whose negation
   overflows int: 6.6p4 makes a constant expression out of its type's
   range a constraint violation, so rcc warns, and the object holds
   the wrapped value, the most negative int. */
static int least = -(-2147483647 - 1);

int
main(void)
{
        return least < 0 ? 0 : 1;
}
