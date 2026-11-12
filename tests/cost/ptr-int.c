/* backend.md 6.3 and aarch64.md 6.3, (T) a between a pointer and an
   integer: nothing at 64 bits, and an int widened first.

   On x86-64 the listing must show to_long() and from_long() as rdi
   stored to the slot with movq and loaded back into rax, nothing
   between; and from_int() widening the int with movslq %eax, %rax.

   On aarch64 the listing must show to_long() and from_long() as x0
   stored to the slot with str and loaded back with ldr, nothing
   between; and from_int() widening the int with sxtw x0, w0. */
long to_long(int *p)
{
    return (long) p;
}

int *from_long(long l)
{
    return (int *) l;
}

int *from_int(int i)
{
    return (int *) i;
}
