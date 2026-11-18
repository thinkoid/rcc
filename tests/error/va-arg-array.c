/* va_arg of an array type, which no argument has after its decay,
   6.3.2.1p3: an error. */
#include <stdarg.h>

int
sum(int count, ...)
{
        va_list ap;
        int s = 0;

        va_arg(ap, int[2]);

        return s;
}

int
main(void)
{
        return sum(0);
}
