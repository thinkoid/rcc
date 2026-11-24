/* shape.md chapter 5, a compound assignment to a _Bool whose address
   reads a volatile object, *p |= x on a _Bool *volatile p: the read of
   p is a side effect, 5.1.2.4p2, which the source writes once, so p's
   value is held in a temporary and the tree is (t = p, *t = (_Bool) (*t
   | x)).  p is loaded once.

   On x86-64, movq p(%rip), %rax; movq %rax, -16(%rbp), then movq
   -16(%rbp), %rax for the store, movq -16(%rbp), %rcx; movzbl (%rcx),
   %ecx for the read, orl -4(%rbp), %ecx, the conversion and movb %cl,
   (%rax): two instructions more than the listing through a _Bool *p,
   which loads p twice.

   On aarch64, adrp x16, p; ldr x0, [x16, :lo12:p]; str x0, [x29, #24],
   then ldr x0, [x29, #24] and ldr x1, [x29, #24]; ldrb w1, [x1], the
   orr, cmp; cset and strb w1, [x0]: one instruction more than the
   listing through a plain pointer. */
_Bool *volatile p;

void
mark(int x)
{
        *p |= x;
}
