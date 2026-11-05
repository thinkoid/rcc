/* Chapter 3, (T) a: trunc, sext, zext, or nothing; movsx, movzx, or
   nothing.  The first slice has no cast expression, so the
   conversions are those of initialization.  The listing must show
   signed_() storing the low byte of a, loading it back with movzbl
   and sign-extending it with movsbl, then widening int to long with
   cltq; unsigned_() loading the byte with movzbl and zero-extending
   it with a second movzbl; and same(), int to unsigned, with no
   instruction between the load and the store. */
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
