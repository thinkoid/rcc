/* cost.md chapter 1, noinline: the attribute asks that calls of the
   function not be inlined, and c1 writes each call as it is written
   and inlines none, so it is dropped and writes nothing (shape.md
   chapter 5).  step() is declared noinline, as a program marks a
   function it wants called; count() calls it.

   On x86-64 the listing must be, byte for byte but for the .file line,
   the listing of this program without the attribute.

   On aarch64 the listing must be the same twin's listing, byte for
   byte but for the .file line. */
static int __attribute__((noinline))
step(int n)
{
        return n + 1;
}

int
count(int n)
{
        return step(n);
}
