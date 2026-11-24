/* shape.md chapter 5, a compound assignment of a bit-field that does
   not fill its access unit through a place with a side effect,
   f[i++].level += v: the place's address is evaluated once into a
   temporary, first (G13), and the update is (a = &f[i++].u, t = *a, *a
   = (((F (t) + v) << s) & M) | (t & ~M)), i stepped once, the unit
   loaded once into t and stored once.

   On x86-64, i's step, then the address leaq (%rax,%rdx,8), %rax; addq
   $4, %rax stored in the temporary, movq %rax, -32(%rbp); movq
   -32(%rbp), %rax; movzbl (%rax), %eax; movb %al, -17(%rbp), the unit
   into t, movq -32(%rbp), %rax for the store, the field's read of t,
   addl -16(%rbp), %ecx, the shift, the and and the or, and movb %cl,
   (%rax).

   On aarch64, i's step, add x0, x0, #4; str x0, [x29, #40] into the
   temporary, ldr x0, [x29, #40]; ldrb w0, [x0]; strb w0, [x29, #32]
   into t, ldr x0, [x29, #40] for the store, the field's read of t, the
   add of v, and strb w1, [x0]. */
struct flags
{
        int count;
        unsigned dirty : 1, level : 3;
};

void
raise(struct flags *f, int i, int v)
{
        f[i++].level += v;
}
