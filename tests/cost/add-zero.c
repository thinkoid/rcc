/* cost.md chapter 1, no folding: a + 0 is an add.  The listing must
   show a loaded into eax and addl $0, %eax, the constant an
   immediate, and not a bare move. */
int add_zero(int a)
{
    return a + 0;
}
