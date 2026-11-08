/* cost.md chapter 1, no branch to conditional move: a < b ? a : b is
   jump code.  The listing must show cmpl %ecx, %eax and a jge to the
   arm that loads b, the arm that loads a into the same eax and jumps
   past it, no setl and no cmov. */
int select(int a, int b)
{
    return a < b ? a : b;
}
