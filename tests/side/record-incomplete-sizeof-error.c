/* An incomplete type has no size, 6.5.4.4p1. */
struct s;

unsigned long
f(void)
{
        return sizeof(struct s);
}
