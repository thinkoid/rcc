/* alloca, which alloca.h defines as GNU C's __builtin_alloca, an
   allocation in the caller's frame: rcc does not compile it yet. */
#include <alloca.h>

int
main(void)
{
        char *p = alloca(16);

        p[0] = 0;
        return p[0];
}
