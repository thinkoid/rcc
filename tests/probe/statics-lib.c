/* The statics probe's library, compiled by clang alone and linked
   twice, once as it is for rcc's build and once renamed under the
   prefix ref_ for the reference's.  It gives a static object and a
   static function the names statics.c gives its own, and reads its
   own: the two files' are four entities, each file's local to it.
   See statics.h. */

#include "statics.h"

static int shared = 2;

static int
same_name(void)
{
        return 20;
}

int
lib_shared(void)
{
        return shared;
}

int
lib_same_name(void)
{
        return same_name();
}
