/* shape.md chapter 2: a #pragma GCC visibility push(default) region
   declares what the program declares without it, since default
   visibility is what rcc writes, so clang's unwind.h, which brackets
   its declarations in such a region, is read.  A region of another
   visibility leaves a typedef and a function of internal linkage as
   they are, since only a symbol of external linkage takes the
   visibility, as clang has it.

   On x86-64 the listing must be, byte for byte but for the .file line,
   the listing of this program without its #include and its pragmas.

   On aarch64 the listing must be the same twin's listing, byte for
   byte but for the .file line. */
#include <unwind.h>

#pragma GCC visibility push(default)
int shown = 1;

int
shown_value(void)
{
        return shown;
}
#pragma GCC visibility pop

#pragma GCC visibility push(hidden)
typedef int count_t;

static count_t
twice(count_t n)
{
        return n + n;
}
#pragma GCC visibility pop

int
twice_shown(void)
{
        return twice(shown);
}
