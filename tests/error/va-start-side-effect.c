/* va_start of a list with a side effect, lists[s++]: the tree writes
   each member of the list, so each would repeat it: not yet. */
#include <stdarg.h>

va_list lists[2];
int
sum(int count, ...)
{
        va_list ap;
        int s = 0;

        va_start(lists[s++], count);

        return s;
}

int
main(void)
{
        return sum(0);
}
