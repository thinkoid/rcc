/* backend.md 7.2 and aarch64.md 7.2, main's return of 0, as
   main-return.c: the last statement is a goto back to a label of the
   loop it builds, whose jump is the last instruction.

   On x86-64 the listing must show jmp .Lmain.2, the goto's, followed at
   once by .Lmain.1, with no movl $0, %eax.

   On aarch64 the listing must show b .Lmain.2 followed at once by
   .Lmain.1, with no mov w0, #0. */
int g;

int
main(void)
{
again:
        g = g + 1;
        if (g == 10)
                return g;
        goto again;
}
