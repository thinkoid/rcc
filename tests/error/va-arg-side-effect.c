/* va_arg of a list with a side effect, lists[s++]: the tree reads the
   list more than once, so each read would repeat it: not yet. */
#include <stdarg.h>

va_list lists[2];
int
sum(int count, ...)
{
        va_list ap;
        int s = 0;

        s = va_arg(lists[s++], int);

        return s;
}

int
main(void)
{
        return sum(0);
}
