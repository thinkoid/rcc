/* G14: a loop runs until its condition says stop. u starts at argc,
   1, and steps by 2, so it stays odd and u % 2 never says stop: the
   loop runs until the oracle's time limit stops it, "timeout". Its
   controlling expression is no constant, and it does no input or
   output, no volatile access and no synchronization, so ISO C lets an
   implementation assume that it ends (6.8.6.1p4): clang -O2 drops the
   loop and returns 0. u comes through argc, so clang drops the loop on
   that assumption alone, not on a value it knows. A loop that writes
   nothing, while (x); with x through argc, does not show the
   divergence: clang -O2 keeps it, a jump to itself after one test of
   x, and hangs as rcc does.

   The listing must show the loop as written, the remainder taken by a
   division at each test and u stepped in its slot: on x86-64 divl,
   cmpl $0, %edx and je out, addl $2, -8(%rbp) and jmp back; on aarch64
   udiv and msub, cbz out, add w0, w0, #2 and str back to the slot, and
   b back. */
int
main(int argc)
{
        unsigned u = argc;

        while (u % 2)
                u += 2;

        return 0;
}
