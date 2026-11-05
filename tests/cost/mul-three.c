/* Chapter 1, no folding: x * 3 is a multiply.  The listing must show
   an imul by 3, not a lea (x,x,2) or a shift and an add. */
int mul_three(int x)
{
    return x * 3;
}
