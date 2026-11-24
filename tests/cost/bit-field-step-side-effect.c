/* shape.md chapter 5, a step of a bit-field that does not fill its
   access unit through a place with a side effect, ++f[i++].level: the
   place's address is evaluated once into a temporary, first, and the
   step is the compound assignment += 1 on the temporary's pointee, i
   stepped once, the unit loaded once and stored once.

   On x86-64, i's step, the address stored in the temporary, movq %rax,
   -24(%rbp); movq -24(%rbp), %rax; movzbl (%rax), %eax; movb %al,
   -13(%rbp), the unit into t, movq -24(%rbp), %rax for the store, the
   field's read of t, addl $1, %ecx, the shift, the and, the or and movb
   %cl, (%rax).

   On aarch64, i's step, str x0, [x29, #32] into the temporary, ldr x0,
   [x29, #32]; ldrb w0, [x0]; strb w0, [x29, #28] into t, ldr x0, [x29,
   #32] for the store, add w1, w1, #1 and strb w1, [x0]. */
struct flags
{
        int count;
        unsigned dirty : 1, level : 3;
};

void
raise(struct flags *f, int i)
{
        ++f[i++].level;
}
