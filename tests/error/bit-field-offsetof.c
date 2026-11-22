/* offsetof of a bit-field: a bit-field has no offset in bytes, and
   7.21p4 leaves its offsetof undefined, so it is an error. */
#include <stddef.h>

struct flags
{
        int count;
        unsigned dirty : 1;
};

int
main(void)
{
        return offsetof(struct flags, dirty);
}
