/* backend.md 7.2 and aarch64.md 7.2, while: the test at the top of the
   loop, a conditional jump out, an unconditional jump back.

   On x86-64 the listing must show the counter loaded and compared with
   cmpl %ecx, %eax and the jge out, then the counter loaded, incremented
   with addl $1, %eax and stored to -8(%rbp) on every iteration, and the
   jmp back to the test.

   On aarch64 the listing must show, with no alignment directive before
   the test, the counter loaded and compared with cmp w0, w1 and the
   b.ge out, then the counter loaded, incremented with add w0, w0, #1
   and stored to [x29, #20] on every iteration, and the b back to the
   test. */
int count(int n)
{
    int i = 0;
    while (i < n)
        i = i + 1;
    return i;
}
