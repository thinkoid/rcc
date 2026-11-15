/* backend.md chapter 8 and shape.md chapter 4, (*p)(x) and p(x): *p on
   a pointer to a function is the function designator, which converts
   back to the pointer where it is called, 6.3.2.1p4, so the two are one
   call and the * writes no instruction.  plain() calls p(x), star()
   (*p)(x) and stars() (**p)(x).

   On x86-64 the listings of star() and stars() must be plain()'s, byte
   for byte but for the function's name: p loaded and stored to the
   callee's slot, x stored to its argument slot and loaded into edi, and
   call *-24(%rbp).

   On aarch64 the listings must be the same: p stored to the callee's
   slot, x loaded into w0, ldr x16 from the callee's slot and
   blr x16. */
int
plain(int (*p)(int), int x)
{
        return p(x);
}

int
star(int (*p)(int), int x)
{
        return (*p)(x);
}

int
stars(int (*p)(int), int x)
{
        return (**p)(x);
}
