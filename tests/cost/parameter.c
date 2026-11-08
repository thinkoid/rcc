/* backend.md 6.3, a parameter: stored from its argument register to
   its slot at entry, loaded from the slot at each read.  The listing
   must show edi and esi stored to -4(%rbp) and -8(%rbp), and a, read
   twice, loaded twice from -4(%rbp). */
int parameter(int a, int b)
{
    return a - b + a;
}
