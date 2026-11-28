/* An array of unknown size has no size, 6.5.4.4p1. */
unsigned long
f(void)
{
        return sizeof(int[]);
}
