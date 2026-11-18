/* backend.md 7.2 and aarch64.md 7.2, main's return of 0, as
   main-return.c: the last statement is an if whose two parts end in a
   jump, so it places no label past its else part and the else part's
   return is the last instruction.

   On x86-64 the listing must show the else part's jmp .Lmain.1
   followed at once by .Lmain.1, with no movl $0, %eax.

   On aarch64 the listing must show b .Lmain.1 followed at once by
   .Lmain.1, with no mov w0, #0. */
int g;

int
main(void)
{
        if (g)
                return 1;
        else
                return 2;
}
