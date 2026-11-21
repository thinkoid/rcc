/* A macro of stdatomic.h, which calls a builtin function of clang's
   atomic operations: the operation is not yet. */
#include <stdatomic.h>

int
set(atomic_flag *p)
{
        return atomic_flag_test_and_set(p);
}
