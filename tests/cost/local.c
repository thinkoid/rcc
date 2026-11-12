/* backend.md 6.3 and aarch64.md 6.3, a local: one stack slot, a store
   at each write and a load at each read; x stays in memory between the
   statements.

   On x86-64 the listing must show x stored to its slot at -8(%rbp) at
   the initializer and at the assignment, and loaded from it at each of
   the two reads, x + 1 as addl $1, %eax.

   On aarch64 the listing must show x stored to its slot at [x29, #20]
   at the initializer and at the assignment, and loaded from it at each
   of the two reads, x + 1 as add w0, w0, #1. */
int local(int a)
{
    int x = a;
    x = x + 1;
    return x;
}
