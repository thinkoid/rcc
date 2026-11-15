/* A program with signal.h, the system's: signal installs the program's
   own handler, whose address the program passes as the header's
   __sighandler_t, a pointer to a function; raise delivers SIGUSR1, and
   the C library calls the handler through that pointer.  The subset has
   no object outside a function's frame for the handler to write, so the
   handler's effect is the exit status itself: it ends the program by
   _Exit, of stdlib.h, which a handler may call, with 42 when the signal
   it was passed is SIGUSR1.  A main that returns has the status 3, and
   one whose signal() failed 2.

   Under rcc and under clang -O2 it exits 42: nothing here is undefined,
   and the row is the same on both sides.

   On x86-64 the listing must show the address of handler, a function
   the file defines, formed PC-relative, leaq handler(%rip), %rax,
   stored to its argument slot, loaded into rsi, and call signal@PLT;
   the result compared with SIG_ERR at 64 bits, cmpq $-1, %rax; and
   call raise@PLT.  handler's listing must show call _Exit@PLT.

   On aarch64 the listing must show the address formed PC-relative,
   adrp x0, handler and add x0, x0, :lo12:handler, stored to its
   argument slot, loaded into x1, and bl signal; the result compared
   with SIG_ERR at 64 bits, cmp x0, #-1; and bl raise.  handler's
   listing must show bl _Exit. */
#include <signal.h>
#include <stdlib.h>

void
handler(int sig)
{
        _Exit(sig == SIGUSR1 ? 42 : 1);
}

int
main(void)
{
        if (signal(SIGUSR1, handler) == SIG_ERR)
                return 2;

        raise(SIGUSR1);

        return 3;
}
