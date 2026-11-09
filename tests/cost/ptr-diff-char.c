/* backend.md 6.3, p - q on char *: the size is 1, so the sub is the
   whole of it.  The listing must show subq %rcx, %rax of the two
   addresses and no cqto and no idiv. */
long diff(char *p, char *q)
{
    return p - q;
}
