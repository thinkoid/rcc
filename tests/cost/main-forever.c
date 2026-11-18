/* backend.md 7.2 and aarch64.md 7.2, main's return of 0 after a loop
   no jump leaves: the last statement is a for (;;) left only by its
   return.  No jump names the label past the loop, so the loop does not
   place it, its jump back is the last instruction, and nothing written
   reaches the end of main.

   On x86-64 the listing must show the loop's jmp .Lmain.2 under no
   step's label, which no continue names, followed at once by .Lmain.1,
   with no label past the loop and no movl $0, %eax.

   On aarch64 the listing must show the same, b .Lmain.2 followed at
   once by .Lmain.1, with no mov w0, #0. */
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
