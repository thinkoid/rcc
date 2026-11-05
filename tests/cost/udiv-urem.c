/* Chapter 3, a / b and a % b, unsigned: xor %edx, %edx; div, the
   quotient from eax and the remainder from edx.  The listing must
   show edx cleared and div in both functions, udiv() returning eax
   and urem() moving edx to eax. */
unsigned udiv(unsigned a, unsigned b)
{
    return a / b;
}

unsigned urem(unsigned a, unsigned b)
{
    return a % b;
}
