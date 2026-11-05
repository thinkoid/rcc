/* Chapter 3, a += b: load, convert, op, convert, store.  The listing
   must show compound() loading x, adding b and storing x back, all in
   int; and narrow() loading c with movzbl, sign-extending it with
   movsbl, adding b in int and storing the low byte back. */
int compound(int a, int b)
{
    int x = a;
    x += b;
    return x;
}

int narrow(int a, int b)
{
    char c = a;
    c += b;
    return c;
}
