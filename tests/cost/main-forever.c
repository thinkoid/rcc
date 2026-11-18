/* backend.md 7.2 and aarch64.md 7.2, main's return of 0 kept after a
   loop no jump leaves: the last statement is a for (;;) left only by
   its return.  No path reaches the end of main, but the loop places the
   label past it whether or not a jump names it, so the instruction is
   written all the same: the rule reads what was written, not the
   program's paths.

   On x86-64 the listing must show .Lmain.3, the step's label, and the
   loop's jmp .Lmain.2, then .Lmain.4, the label past the loop, which
   no jump names, then movl $0, %eax before .Lmain.1.

   On aarch64 the listing must show the same labels with b for jmp, and
   mov w0, #0 before .Lmain.1. */
int g;

int
main(void)
{
        for (;;) {
                g = g - 1;
                if (g == 0)
                        return 3;
        }
}
