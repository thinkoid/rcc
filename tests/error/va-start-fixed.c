/* va_start in a function with fixed parameters, which is called with
   no variable arguments to start a list at, 7.16p3: an error. */
#include <stdarg.h>

int
fixed(int count)
{
        va_list ap;

        va_start(ap, count);

        return 0;
}

int
main(void)
{
        return fixed(0);
}
