/* backend.md 6.3 and aarch64.md 6.3, a parameter: stored from its
   argument register to its slot at entry, loaded from the slot at each
   read.

   On x86-64 the listing must show edi and esi stored to -4(%rbp)
   and -8(%rbp), and a, read twice, loaded twice from -4(%rbp).

   On aarch64 the listing must show w0 and w1 stored to [x29, #16]
   and [x29, #20], and a, read twice, loaded twice from [x29, #16]. */
int parameter(int a, int b)
{
    return a - b + a;
}
