/* cost.md chapter 1, __builtin_expect: c is evaluated for its effect
   alone, after e, and a read of a volatile object is a side effect,
   5.1.2.4p2, so __builtin_expect(e, ready) of a volatile ready loads
   ready once, with e's value held in a temporary across the load.

   On x86-64 the listing must be, byte for byte but for the .file line,
   the listing of this program with (t = e, ready, t) in place of
   __builtin_expect(e, ready), t a long local declared first.

   On aarch64 the listing must be the same twin's listing, byte for
   byte but for the .file line. */
volatile int ready;

long
expect_ready(long e)
{
        return __builtin_expect(e, ready);
}
