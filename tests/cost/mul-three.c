/* cost.md chapter 1, no folding: x * 3 is a multiply.  The listing
   must show imull $3, %eax, %eax, the immediate form, not a lea
   (x,x,2) or a shift and an add. */
int mul_three(int x)
{
    return x * 3;
}
