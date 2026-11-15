/* volatile costs nothing (shape.md chapter 5).  c1 writes every access
   the source writes, an unused one included, at the object's width,
   which is what a volatile object asks of the compiler (6.7.4.1p8), so
   the qualifier is dropped as const is.  peek() loads v and *p though
   their values are unused, and loads *p twice for *p + *p; poke()
   stores one byte through a pointer to a volatile char; and spin()
   keeps its register volatile counter in a callee-saved register.

   On x86-64 the listing must be, byte for byte but for the .file line,
   the listing of this program without volatile.

   On aarch64 the listing must be the same twin's listing, byte for
   byte but for the .file line. */
int
peek(volatile int *p)
{
        volatile int v = 1;

        v;
        *p;

        return *p + *p;
}

void
poke(volatile char *port)
{
        *port = 'A';
}

int
spin(int n)
{
        register volatile int i = 0;

        while (i < n)
                i++;

        return i;
}
