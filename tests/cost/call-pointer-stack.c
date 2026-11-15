/* backend.md chapter 8 and aarch64.md chapter 8, a call through a
   pointer with arguments past the convention's registers: the callee's
   slot is written first, the arguments' after it, and the stack
   arguments are placed from their slots as for a named call.  nine()
   passes nine ints through p.

   On x86-64 the listing must show p stored to the callee's slot first,
   each constant argument stored to its slot in one instruction; the
   three past the sixth pushed from their slots last first over an
   8-byte pad, subq $8, %rsp; the first six loaded into edi to r9d; the
   call through the callee's slot, call *-16(%rbp), which the pushes do
   not move, since the slot is off rbp; and addq $32, %rsp.

   On aarch64 the listing must show p stored to the callee's slot
   first, each constant argument stored through w0; the outgoing area
   opened with sub sp, sp, #16; the ninth copied through x17 to [sp];
   the first eight loaded into w0 to w7; then ldr x16 from the callee's
   slot and blr x16; and add sp, sp, #16. */
int
nine(int (*p)(int, int, int, int, int, int, int, int, int))
{
        return p(1, 2, 3, 4, 5, 6, 7, 8, 9);
}
