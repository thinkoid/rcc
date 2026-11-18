/* va_arg of void, which is no object type, 7.16.1.1p2: an error. */
#include <stdarg.h>

int
sum(int count, ...)
{
        va_list ap;
        int s = 0;

        va_arg(ap, void);

        return s;
}

int
main(void)
{
        return sum(0);
}
