/* cost.md chapter 1, no folding: x * 3 is a multiply.

   On x86-64 the listing must show imull $3, %eax, %eax, the immediate
   form, not a lea (x,x,2) or a shift and an add.

   On aarch64, where mul takes no immediate, the listing must show
   mov w1, #3 and mul w0, w0, w1, not an add of a shifted operand or a
   shift and an add. */
int mul_three(int x)
{
    return x * 3;
}
