/* An array's size whose sum overflows int: 6.6p4 makes a constant
   expression out of its type's range a constraint violation, so rcc
   warns, and the array has the size of the wrapped value, 63129088
   elements, as the same arithmetic has at run time. */
int a[(2147483647 + 1) / 4 + 600000000];

int
main(void)
{
        return a[0];
}
