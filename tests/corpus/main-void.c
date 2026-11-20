/* choices.md J.3.3(4): main defined with the return type void, a form
   5.1.2.3.2p1 leaves to the implementation.  rcc compiles the
   definition as written, and main reaching its } returns 0, as a main
   of type int does.  5.1.2.3.4p1 leaves the status of a main of any
   other return type unspecified: clang -O2 ends main with the call of
   putchar as a jump, so it exits with the character written, 10.

   On x86-64 the listing must show call putchar@PLT, then movl $0,
   %eax before .Lmain.1, the exit, where main reaches its }.

   On aarch64 the listing must show bl putchar, then mov w0, #0 before
   .Lmain.1. */
#include <stdio.h>

void
main(void)
{
        putchar('\n');
}
