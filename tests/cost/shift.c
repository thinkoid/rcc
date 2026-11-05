/* Chapter 3, a << n and a >> n: shl, sar for a signed left operand,
   shr for an unsigned one, each by cl.  The listing must show the
   count loaded into ecx and one shl, sar or shr by cl per function, the
   count masked by the instruction and by nothing else. */
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
