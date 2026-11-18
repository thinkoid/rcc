/* offsetof of a name that is no member of the struct, 7.21p4. */
#include <stddef.h>

struct point
{
        int x, y;
};

int
main(void)
{
        return offsetof(struct point, z);
}
