/* cost.md chapter 1, no moving: a loop-invariant expression is computed
   every iteration.  In invariant_constant(), 12 * 34 is an integer
   constant expression, folded in the parser to 408 (backend.md 6.4 and
   aarch64.md 6.4), and its add is in the loop.

   On x86-64 the listing of invariant() must show the loads of a and b,
   cltd and idivl inside the loop, between the test at the top and the
   jmp back.  The held s is in rax and b in rdx when the division comes,
   so both move out, to rdi and rsi, before a moves into eax
   (backend.md 6.5).  The listing of invariant_constant() must show
   addl $408, %eax inside the loop and no imul.

   On aarch64 the listing of invariant() must show the loads of a and b
   and sdiv w1, w1, w2 inside the loop, between the test at the top and
   the b back, on the registers the loads took and with no move
   (aarch64.md 6.5); and of invariant_constant(), add w0, w0, #408
   inside the loop and no mul. */
int invariant(int a, int b, int n)
{
    int s = 0;
    for (int i = 0; i < n; i = i + 1)
        s = s + a / b;
    return s;
}

int invariant_constant(int n)
{
    int s = 0;
    for (int i = 0; i < n; i = i + 1)
        s = s + 12 * 34;
    return s;
}
