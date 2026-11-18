/* backend.md 7.2 and aarch64.md 7.2, main's return of 0 kept where
   main falls off its end: the last statement is an if with no else,
   whose condition's false exit is the label past its body.

   On x86-64 the listing must show the return's jmp .Lmain.1, then
   .Lmain.2, the if's label, and movl $0, %eax before .Lmain.1.

   On aarch64 the listing must show b .Lmain.1, then .Lmain.2 and
   mov w0, #0 before .Lmain.1. */
int g;

int
main(void)
{
        if (g)
                return 1;
}
