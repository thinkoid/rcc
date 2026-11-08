/* backend.md 7.2, while: the test at the top of the loop, a
   conditional jump out, an unconditional jump back.  The listing must
   show the counter loaded and compared with cmpl %ecx, %eax and the
   jge out, then the counter loaded, incremented with addl $1, %eax
   and stored to -8(%rbp) on every iteration, and the jmp back to the
   test. */
int count(int n)
{
    int i = 0;
    while (i < n)
        i = i + 1;
    return i;
}
