/* hot says where a function is laid out and how hard it is optimized,
   as cold does, and artificial describes a function's debugging
   information: each is dropped and writes nothing (shape.md chapter
   5), so a call of a function either marks is the plain call.  warm()
   is declared hot and plain() artificial, and both() calls each.

   On x86-64 the listing must be, byte for byte but for the .file line,
   the listing of this program without the attributes.

   On aarch64 the listing must be the same twin's listing, byte for
   byte but for the .file line. */
int warm(int n) __attribute__((hot));
int plain(int n) __attribute__((__artificial__));

int
both(int n)
{
        return warm(n) + plain(n);
}
