/* shape.md chapter 5, a prefix step of a _Bool through a place with a
   side effect, ++*p++: ++E is E += 1, 6.5.4.1p2, which names E twice,
   and E is evaluated once, so its address, p++, is evaluated once into
   a temporary and the tree is (t = p++, *t = (_Bool) (*t + 1)), p
   stepped once.

   On x86-64, p's step, movq %rcx, -16(%rbp) into the temporary, movq
   -16(%rbp), %rax for the store, movq -16(%rbp), %rcx; movzbl (%rcx),
   %ecx for the read, addl $1, the conversion and movb %cl, (%rax).

   On aarch64, p's step, str x0, [x29, #24] into the temporary, ldr x0,
   [x29, #24] and ldr x1, [x29, #24]; ldrb w1, [x1], add w1, w1, #1,
   cmp; cset and strb w1, [x0]. */
void
mark(_Bool *p)
{
        ++*p++;
}
