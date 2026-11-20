/* cost.md chapter 1, return of a void expression in a void function:
   GNU C takes it, and rcc takes it so, with a warning (shape.md chapter
   5): the expression for its effect, then the return.  tail() returns
   the call of a void function, and pick() does so in an if's body.

   On x86-64 the listing must be, byte for byte but for the .file line,
   the listing of this program with each return e; written as e;
   return; in a block of its own.

   On aarch64 the listing must be the same twin's listing, byte for
   byte but for the .file line. */
void g(void);
void h(int);

void
tail(void)
{
        return g();
}

void
pick(int x)
{
        if (x)
                return h(x);

        g();
}
