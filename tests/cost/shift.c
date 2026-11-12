/* backend.md 6.3 and aarch64.md 6.3, a << n and a >> n: a left shift,
   an arithmetic right shift for a signed left operand and a logical one
   for an unsigned one, each by a count in a register, the count masked
   by the instruction and by nothing else (G3).

   On x86-64 the listing must show a loaded into eax and the count into
   ecx, the register the instruction wants, then one shll %cl, %eax,
   sarl %cl, %eax or shrl %cl, %eax per function.

   On aarch64 the listing must show a loaded into w0 and the count
   into w1, the next free register, then one lsl w0, w0, w1,
   asr w0, w0, w1 or lsr w0, w0, w1 per function. */
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
