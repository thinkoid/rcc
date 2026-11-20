/* cost.md chapter 1, a pointer to void and a pointer to a function
   converted as if by assignment: GNU C takes it either way, and rcc
   takes it so, with a warning (shape.md chapter 5), the conversion a
   cast makes.  install() assigns a void * to a pointer to a function,
   and erase() returns a pointer to a function as a void *.

   On x86-64 the listing must be, byte for byte but for the .file line,
   the listing of this program with each conversion written as a cast,
   (void (*)(int)) p and (void *) h.

   On aarch64 the listing must be the same twin's listing, byte for
   byte but for the .file line. */
void (*handler)(int);

void
install(void *p)
{
        handler = p;
}

void *
erase(void (*h)(int))
{
        return h;
}
