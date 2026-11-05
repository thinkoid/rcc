/* Chapter 1, no deleting; chapter 2, -disable-cgp.  An if with an
   empty body is still a test and a branch.  The listing must show a
   loaded and compared with 0, a je and a jmp to the same place, and
   only then the second if; CodeGenPrepare would remove the empty
   block and the test with it. */
int empty_if(int a)
{
    int x = 0;
    if (a) {
    }
    if (a > 1)
        x = 1;
    return x;
}
