/* shape.md chapter 5, a compound assignment to a _Bool through a place
   with a side effect, *p++ |= 1: b op= e is b = (_Bool) (b op (e)),
   6.5.17.3p4, which names b twice, and b is evaluated once, so its
   address, p++, is evaluated once into a temporary and the tree is (t =
   p++, *t = (_Bool) (*t | 1)), p stepped once.

   On x86-64, p's step, movq %rax, %rcx; addq $1, %rax; movq %rax,
   -8(%rbp), then movq %rcx, -16(%rbp) into the temporary, movq
   -16(%rbp), %rax for the store, movq -16(%rbp), %rcx; movzbl (%rcx),
   %ecx for the read, orl $1, the conversion and movb %cl, (%rax).

   On aarch64, p's step, add x1, x0, #1; str x1, [x29, #16], then str
   x0, [x29, #24] into the temporary, ldr x0, [x29, #24] for the store,
   ldr x1, [x29, #24]; ldrb w1, [x1] for the read, the orr, cmp; cset
   and strb w1, [x0]. */
void
mark(_Bool *p)
{
        *p++ |= 1;
}
