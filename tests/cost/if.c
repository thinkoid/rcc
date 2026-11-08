/* backend.md 7.2, if, and 7.1, jump code: the condition is a compare
   and a jump, with no value.  The listing must show a < b as
   cmpl %ecx, %eax and a jge past the then-block, no setl, the
   then-block's store of a to x's slot and its jmp over the
   else-block, and the else-block's store of b. */
int choose(int a, int b)
{
    int x;
    if (a < b)
        x = a;
    else
        x = b;
    return x;
}
