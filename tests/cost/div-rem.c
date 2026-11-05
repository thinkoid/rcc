/* Chapter 3, a / b and a % b, signed: cltd; idiv, the quotient from
   eax and the remainder from edx.  The listing must show cltd and
   idiv in both functions, sdiv() returning eax as it stands and
   srem() moving edx to eax. */
int sdiv(int a, int b)
{
    return a / b;
}

int srem(int a, int b)
{
    return a % b;
}
