/* backend.md 7.2 and aarch64.md 7.2, main's return of 0 kept after a
   loop: the last statement is a while whose body ends in
   if (g > 5) break; else return 3;.  The loop places the label past it,
   and the break jumps there, so the end of main is reached and the
   instruction is written.

   On x86-64 the listing must show jg .Lmain.3, the break, and after
   the loop's jmp .Lmain.2 the label .Lmain.3, then movl $0, %eax
   before .Lmain.1.

   On aarch64 the listing must show b.gt .Lmain.3, then after b .Lmain.2
   the label .Lmain.3 and mov w0, #0 before .Lmain.1. */
int g;

int
main(void)
{
        while (g) {
                g = g - 1;
                if (g > 5)
                        break;
                else
                        return 3;
        }
}
