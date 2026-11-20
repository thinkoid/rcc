/* cost.md chapter 1, __builtin_expect: likely() and unlikely(), as a
   program defines them, are __builtin_expect(e, c), which is e
   converted to long and says only which value e is likely to have, so
   it writes nothing of its own.  c is evaluated for its effect alone,
   after e, and a constant has none.  pick() tests its argument through
   unlikely(), and wide() returns likely()'s value.

   On x86-64 the listing must be, byte for byte but for the .file line,
   the listing of this program with (long)(e) in place of each
   __builtin_expect(e, c).

   On aarch64 the listing must be the same twin's listing, byte for
   byte but for the .file line. */
#define likely(x)   __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

int
pick(int x)
{
        if (unlikely(x < 0))
                return 0;

        return x;
}

long
wide(int x)
{
        return likely(x);
}
