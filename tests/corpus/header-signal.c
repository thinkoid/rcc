/* A program with signal.h, the system's, written in GNU C, which rcc
   reads as it is: typedef void (*__sighandler_t)(int), the type of a
   handler, a pointer to a function; signal, which takes one and
   returns one; and SIG_IGN, SIG_DFL and SIG_ERR, each an integer cast
   to it.  The program takes no function's address.  raise(0) delivers
   nothing and returns 0; signal(SIGUSR1, SIG_IGN) returns the handler
   before it, SIG_DFL; raise(SIGUSR1) then delivers a signal that is
   ignored, and the program lives; and the next signal() returns the
   SIG_IGN the first one set.  So its exit status says that the
   declarations the header gave it are the functions it called, and
   that a pointer to a function went in and came back as a value.

   Under rcc and under clang -O2 it exits 0: nothing here is undefined,
   and the row is the same on both sides.

   On x86-64 the listing must show call raise@PLT and call signal@PLT;
   SIG_IGN and SIG_DFL stored as the 64-bit constants they are, movq $1
   and movq $0, and passed in rsi as any pointer; each result compared
   with them at 64 bits, cmpq $0, %rax and cmpq $1, %rax; and nothing
   for any declaration of the header.

   On aarch64 the listing must show bl raise and bl signal; SIG_IGN and
   SIG_DFL stored as the 64-bit constants they are, mov x0, #1 and str
   xzr, and passed in x1 as any pointer; each result compared with them
   at 64 bits, cmp x0, #0 and cmp x0, #1; and nothing for any
   declaration of the header. */
#include <signal.h>

int
main(void)
{
        if (raise(0) != 0)
                return 1;

        if (signal(SIGUSR1, SIG_IGN) != SIG_DFL)
                return 2;

        if (raise(SIGUSR1) != 0)
                return 3;

        if (signal(SIGUSR1, SIG_DFL) != SIG_IGN)
                return 4;

        return 0;
}
