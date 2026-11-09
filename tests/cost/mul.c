/* backend.md 6.3, a * b: imul.  The listing must show a loaded into
   eax and b into ecx, and one imull %ecx, %eax. */
int mul(int a, int b)
{
    return a * b;
}
