/* backend.md 7.2 and aarch64.md 7.2, e ? a : b for its value: jump
   code, never a conditional move.

   On x86-64 the listing must show a compared with cmpl $0, %eax, a je
   to the arm that loads c, the arm that loads b into the same eax and
   jumps past it, the two joining at the jmp to the epilogue label, and
   no cmov.

   On aarch64 the listing must show a tested with cbz w0 to the arm that
   loads c, the arm that loads b into the same w0 and branches past it,
   the two joining at the b to the epilogue label, and no csel. */
int conditional(int a, int b, int c)
{
    return a ? b : c;
}
