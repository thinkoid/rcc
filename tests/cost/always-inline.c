/* cost.md chapter 1, always_inline: the attribute asks that each call
   of the function be inlined, and c1 inlines none, so it is dropped,
   with a warning, and writes nothing (shape.md chapter 5): the call is
   written.  step() is declared always_inline, as a program marks a
   function it wants inlined; count() calls it.

   On x86-64 the listing must be, byte for byte but for the .file line,
   the listing of this program without the attribute.

   On aarch64 the listing must be the same twin's listing, byte for
   byte but for the .file line. */
static inline int __attribute__((always_inline))
step(int n)
{
        return n + 1;
}

int
count(int n)
{
        return step(n);
}
