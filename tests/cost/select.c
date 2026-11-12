/* cost.md chapter 1, no branch to conditional move: a < b ? a : b is
   jump code.

   On x86-64 the listing must show cmpl %ecx, %eax and a jge to the arm
   that loads b, the arm that loads a into the same eax and jumps past
   it, no setl and no cmov.

   On aarch64 the listing must show cmp w0, w1 and a b.ge to the arm
   that loads b, the arm that loads a into the same w0 and branches past
   it, no cset and no csel. */
int select(int a, int b)
{
    return a < b ? a : b;
}
