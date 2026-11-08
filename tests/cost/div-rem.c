/* backend.md 6.3, a / b and a % b, signed: cltd and idiv, the
   quotient from eax and the remainder from edx.  The listing must
   show a loaded into eax and b into ecx, cltd and idivl %ecx in both
   functions, sdiv() returning eax as it stands and srem() moving edx
   to eax with movl %edx, %eax. */
int sdiv(int a, int b)
{
    return a / b;
}

int srem(int a, int b)
{
    return a % b;
}
