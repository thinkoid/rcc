/* va_arg of a double, a floating type: not yet. */
#include <stdarg.h>

int
sum(int count, ...)
{
        va_list ap;
        int s = 0;

        s = (int) va_arg(ap, double);

        return s;
}

int
main(void)
{
        return sum(0);
}
