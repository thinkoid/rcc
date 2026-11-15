/* A pointer to a function is a pointer (shape.md chapter 3): eight
   bytes, an address, loaded, stored, passed, returned and compared as
   any pointer is, with no instruction of its own.  The program takes no
   function's address and calls through no pointer.

   On x86-64 the listing must be, byte for byte but for the .file line,
   the listing of this program with void * for every handler_t and
   without the typedef.

   On aarch64 the listing must be the same twin's listing, byte for
   byte but for the .file line. */
typedef void (*handler_t)(int);

handler_t install(int sig, handler_t h);

handler_t
swap(int sig, handler_t h)
{
        handler_t old = install(sig, h);

        if (old == h || !old)
                return 0;

        return old;
}
