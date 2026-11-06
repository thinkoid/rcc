/* Chapter 3, p - q on char *: the size is 1, so the IR has no sdiv.
   The listing must show the sub of the two addresses and no cqto and
   no idiv. */
long diff(char *p, char *q)
{
    return p - q;
}
