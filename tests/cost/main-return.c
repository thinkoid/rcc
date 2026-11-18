/* backend.md 7.2 and aarch64.md 7.2, main's return of 0: main writes
   it before the epilogue unless its body ends in a jump, the last
   instruction an unconditional jump with no label placed after it.
   Here the last statement is a return, whose jump to the epilogue is
   the last instruction.

   On x86-64 the listing must show the return's jmp .Lmain.1 followed
   at once by .Lmain.1, with no movl $0, %eax.

   On aarch64 the listing must show the same, b .Lmain.1 followed at
   once by .Lmain.1, with no mov w0, #0. */
int g;

int
main(void)
{
        g = g + 1;
        return g;
}
