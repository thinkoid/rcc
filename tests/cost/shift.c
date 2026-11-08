/* backend.md 6.3, a << n and a >> n: shl, sar for a signed left
   operand, shr for an unsigned one, each by cl.  The listing must
   show a loaded into eax and the count into ecx, the register the
   instruction wants, then one shll %cl, %eax, sarl %cl, %eax or
   shrl %cl, %eax per function, the count masked by the instruction
   and by nothing else. */
int shl(int a, int n)
{
    return a << n;
}

int sar(int a, int n)
{
    return a >> n;
}

unsigned shr(unsigned a, int n)
{
    return a >> n;
}
