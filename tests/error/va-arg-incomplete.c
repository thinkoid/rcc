/* va_arg of a struct that is incomplete, which is no object type yet,
   7.16.1.1p2: an error. */
#include <stdarg.h>

struct opaque;
int
sum(int count, ...)
{
        va_list ap;
        int s = 0;

        va_arg(ap, struct opaque *);
        va_arg(ap, struct opaque);

        return s;
}

int
main(void)
{
        return sum(0);
}
