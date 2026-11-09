/* backend.md 7.2, if: an if with an empty body is still a test and a
   branch.  The listing must show a loaded, compared with
   cmpl $0, %eax and a je to the label that follows at once, and only
   then the second if, a compared with cmpl $1, %eax and a jle past
   the store of 1 to x's slot. */
int empty_if(int a)
{
    int x = 0;
    if (a) {
    }
    if (a > 1)
        x = 1;
    return x;
}
