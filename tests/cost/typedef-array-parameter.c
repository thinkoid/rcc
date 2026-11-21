/* shape.md chapter 5: a typedef of an array whose size rcc cannot
   compute, here by the size of a struct with a bit-field, names a type
   rcc does not use yet, which keeps the array's element, so a
   parameter of the type is adjusted to a pointer to the element,
   6.7.7.4p6.  second() is declared with such a parameter and with a
   long *, which is one function.

   On x86-64 the listing must be, byte for byte but for the .file line,
   the listing of this program with long * for each parameter of the
   typedef's type.

   On aarch64 the listing must be the same twin's listing, byte for
   byte but for the .file line. */
struct regs
{
        long r[2];
        unsigned live : 1;
};

typedef long regs_t[sizeof(struct regs)];

long second(regs_t set);
long second(long *set);

long
second(regs_t set)
{
        return set[1];
}
