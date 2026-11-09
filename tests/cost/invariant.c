/* cost.md chapter 1, no moving: a loop-invariant expression is
   computed every iteration.  The listing of invariant() must show the
   loads of a and b, cltd and idivl inside the loop, between the test
   at the top and the jmp back.  The held s is in rax and b in rdx
   when the division comes, so both move out, to rdi and rsi, before a
   moves into eax (backend.md 6.5).  In invariant_constant(), 12 * 34
   is an integer constant expression, folded in the parser to 408
   (backend.md 6.4): the listing must show addl $408, %eax inside the
   loop and no imul. */
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
