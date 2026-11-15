/* cost.md chapter 1, inline: a static inline function of the program's
   own is compiled as the static function it is.  inline suggests that
   calls of the function be fast (6.7.5p6), and c1 writes each call as
   it is written either way, so the specifier writes nothing.  max() is
   static inline, and pick() calls it.

   On x86-64 the listing must be, byte for byte but for the .file line,
   the listing of this program without inline.

   On aarch64 the listing must be the same twin's listing, byte for
   byte but for the .file line. */
static inline int
max(int a, int b)
{
        return a > b ? a : b;
}

int
pick(int a, int b)
{
        return max(a, b);
}
