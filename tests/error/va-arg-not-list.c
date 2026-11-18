/* va_arg of an int, which is no va_list, 7.16.1.1p1: an error. */
#include <stdarg.h>

int
sum(int count, ...)
{
        va_list ap;
        int s = 0;

        s = va_arg(s, int);

        return s;
}

int
main(void)
{
        return sum(0);
}
