/* va_end of an int, which is no va_list, 7.16.1.3p1: an error. */
#include <stdarg.h>

int
sum(int count, ...)
{
        va_list ap;
        int s = 0;

        va_end(s);

        return s;
}

int
main(void)
{
        return sum(0);
}
