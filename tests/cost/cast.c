/* backend.md 6.3, (T) a, and the width rule.  The conversions are
   those of initialization.  The listing must show signed_() storing
   the low byte of a with movb %al, -5(%rbp), loading it back with
   movsbl, which is the whole of char to int, and widening int to long
   with movslq %eax, %rax; unsigned_() storing the byte the same way
   and loading it with movzbl, which is the whole of unsigned char to
   unsigned; and same(), int to unsigned, with no instruction between
   the load and the store. */
long signed_(int a)
{
    char c = a;
    int i = c;
    long l = i;
    return l;
}

unsigned unsigned_(int a)
{
    unsigned char c = a;
    unsigned u = c;
    return u;
}

unsigned same(int a)
{
    unsigned u = a;
    return u;
}
