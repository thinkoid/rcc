/* backend.md 7.2, e ? a : b for its value: jump code, never cmov.
   The listing must show a compared with cmpl $0, %eax, a je to the
   arm that loads c, the arm that loads b into the same eax and jumps
   past it, the two joining at the jmp to the epilogue label, and no
   cmov. */
int conditional(int a, int b, int c)
{
    return a ? b : c;
}
