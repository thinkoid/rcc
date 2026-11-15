/* backend.md chapter 8 and aarch64.md chapter 8, p(a, b): the call
   through a pointer.  The callee is a hidden local of its call, as each
   argument is: p is loaded and stored to a slot of its own first, since
   it is written first, then each argument to its slot, then the slots
   loaded into the argument registers.

   On x86-64 the listing must show p loaded into rax and stored to the
   callee's slot, movq %rax, -24(%rbp); b and then a each stored to an
   argument slot of its own; those loaded into edi and esi; and the call
   through the callee's slot, call *-24(%rbp), with no register; the
   result left in eax.

   On aarch64 the listing must show p stored to the callee's slot,
   str x0, [x29, #32]; b and then a each stored to an argument slot of
   its own; those loaded into w0 and w1; then the callee's slot loaded
   into x16, ldr x16, [x29, #32], and blr x16; the result left in
   w0. */
int
call(int (*p)(int, int), int a, int b)
{
        return p(b, a);
}
