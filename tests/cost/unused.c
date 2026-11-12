/* backend.md 6.1 and aarch64.md 6.1, the rule: an expression statement
   is written like any other expression, though its value is unused.
   a / b is performed, and does what the target's division does (G4).

   On x86-64 the listing must show a + 0 as a loaded into eax and
   addl $0, %eax, and a / b as a and b loaded, cltd and idivl %ecx, then
   movl $0, %eax for the return: the division faults when b is 0 or when
   a is INT_MIN and b is -1.

   On aarch64 the listing must show a + 0 as a loaded into w0 and
   add w0, w0, #0, and a / b as a and b loaded and sdiv w0, w0, w1, then
   mov w0, #0 for the return: the division does not trap, and yields 0
   when b is 0 and INT_MIN when a is INT_MIN and b is -1
   (aarch64.md chapter 9). */
int unused(int a, int b)
{
    a + 0;
    a / b;
    return 0;
}
