/* A static assertion writes nothing.  It declares nothing, 6.7.12p3,
   so the tree keeps nothing of one, at file scope, in a block or as a
   for's first clause, with its string or without, and spelled
   static_assert through assert.h, whose macro spells it _Static_assert
   in the dialect rcc preprocesses in.

   On x86-64 the listing must be, byte for byte but for the .file line,
   the listing of this program without its #include and its
   assertions, a for's first clause left empty.

   On aarch64 the listing must be the same twin's listing, byte for
   byte but for the .file line. */
#include <assert.h>

_Static_assert(sizeof(int) == 4, "int is 32 bits");
static_assert(sizeof(long) == 8, "long is 64 bits");

int
count(int n)
{
        int s = 0;

        _Static_assert(sizeof s == 4);
        for (_Static_assert(1, "a for's first clause"); s < n; s++) {
                static_assert(-1 < 0, "int is signed");
        }

        return s;
}
