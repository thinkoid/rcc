/* choices.md J.3.3(4): main defined with a third parameter, the
   environment, a form 5.1.2.3.2p1 leaves to the implementation.  rcc
   compiles the definition as written: envp is the third argument the
   C library passes, the same pointer as environ, and its walk ends at
   a null pointer.  argc is 1 and argv[argc] null, as the oracle runs
   the program with no arguments.  A failed check exits 100 plus its
   number; with none, main reaches its }, which returns 0.

   Under rcc and under clang -O2 it exits 0: nothing here is
   undefined.

   On x86-64 the listing must show main storing its three parameters
   in three slots, movl %edi, -4(%rbp), movq %rsi, -16(%rbp) and
   movq %rdx, -24(%rbp), and movl $0, %eax before .Lmain.1, the exit,
   where main reaches its }.

   On aarch64 the listing must show str w0, [x29, #16], str x1, [x29,
   #24] and str x2, [x29, #32], and mov w0, #0 before .Lmain.1. */
extern char **environ;

int
main(int argc, char **argv, char **envp)
{
        int n = 0;

        if (argc != 1 || argv[argc] != 0)
                return 101;
        if (envp != environ)
                return 102;

        while (envp[n] != 0)
                n++;

        if (environ[n] != 0)
                return 103;
}
