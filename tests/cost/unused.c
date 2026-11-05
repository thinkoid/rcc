/* Chapter 4, an unused result is not computed.  a + 0; and a / b; as
   statements emit nothing: the listing must show the stores of the
   parameters to their slots and the return of 0, with no add and no
   idiv, so the division cannot fault. */
int unused(int a, int b)
{
    a + 0;
    a / b;
    return 0;
}
