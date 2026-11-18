/* va_arg of a struct, which passes by value, 7.16.1.1p2: not yet. */
#include <stdarg.h>

struct point
{
        int x, y;
};
int
sum(int count, ...)
{
        va_list ap;
        int s = 0;

        s = va_arg(ap, struct point).x;

        return s;
}

int
main(void)
{
        return sum(0);
}
