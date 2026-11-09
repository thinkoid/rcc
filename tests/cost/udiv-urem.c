/* backend.md 6.3, a / b and a % b, unsigned: edx cleared and div, the
   quotient from eax and the remainder from edx.  The listing must
   show xorl %edx, %edx and divl %ecx in both functions, udiv()
   returning eax as it stands and urem() moving edx to eax with
   movl %edx, %eax. */
unsigned udiv(unsigned a, unsigned b)
{
    return a / b;
}

unsigned urem(unsigned a, unsigned b)
{
    return a % b;
}
