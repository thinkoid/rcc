/* backend.md chapter 5 and aarch64.md 5.2, a parameter past the
   convention's registers: the caller stored it above the callee's
   frame, where the callee reads it.  ten() takes ten parameters, the
   ninth a char and the tenth a long, and adds the first, the ninth
   and the tenth.

   On x86-64 the listing must show the six register parameters stored
   to their slots in the prologue, and i and j read where the caller
   pushed them, movsbl 32(%rbp), %ecx and movq 40(%rbp), %rcx, with no
   copy.

   On aarch64 the listing must show, after mov x29, sp and before the
   stores of the eight register parameters, each stack parameter
   copied through x17 to a slot of its own at its width: ldrb w17,
   [x29, #64], at the frame's size, and strb w17, [x29, #48] for i;
   ldr x17, [x29, #72] and str x17, [x29, #56] for j.  The body then
   loads both from their slots, i with ldrb. */
long ten(int a, int b, int c, int d, int e, int f, int g, int h,
         char i, long j)
{
    return a + i + j;
}
