/* cost.md chapter 1, no folding: a + 0 is an add.

   On x86-64 the listing must show a loaded into eax and addl $0, %eax,
   the constant an immediate, and not a bare move.

   On aarch64 the listing must show a loaded into w0 and add w0, w0, #0,
   the constant an immediate, and not a bare move. */
int add_zero(int a)
{
    return a + 0;
}
