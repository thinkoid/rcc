/* va_start in a variadic definition, which builds the list from the
   function's save area: the parser builds it, and c1 does not write
   the save area yet. */
#include <stdarg.h>

int
sum(int count, ...)
{
        va_list ap;
        int s = 0;

        va_start(ap, count);
        s = va_arg(ap, int);
        va_end(ap);

        return s;
}

int
main(void)
{
        return sum(0);
}
