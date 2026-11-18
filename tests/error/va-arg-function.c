/* va_arg of a function type, which is no object type, 7.16.1.1p2:
   an error. */
#include <stdarg.h>

int
sum(int count, ...)
{
        va_list ap;
        int s = 0;

        va_arg(ap, int(void));

        return s;
}

int
main(void)
{
        return sum(0);
}
