/* shape.md chapter 5, a store into a bit-field that does not fill its
   access unit through a place with a side effect, f[i++].level: the
   tree names the place twice, so its address, &f[i++].u, is evaluated
   once into a temporary, and the store is (t = &f[i++].u, *t = ((v <<
   s) & M) | (*t & ~M)), i stepped once.  A store of 1, mark's, is one
   compound assignment that names the place once and needs no temporary.

   On x86-64, mark is i's step and orb $1, 4(%rax,%rdx,8).  raise is i's
   step, the address leaq (%rax,%rdx,8), %rax; addq $4, %rax and movq
   %rax, -24(%rbp) into the temporary, then movq -24(%rbp), %rax for the
   store and movq -24(%rbp), %rdx; movzbl (%rdx), %edx for the unit's
   one load, the and with -15, the or and the one store movb %cl,
   (%rax).

   On aarch64, mark is i's step and ldrb w1, [x0, #4]; orr; strb.  raise
   is i's step, add x0, x0, w1, sxtw #3; add x0, x0, #4; str x0, [x29,
   #32] into the temporary, then ldr x0, [x29, #32] for the store and
   ldr x2, [x29, #32]; ldrb w2, [x2] for the unit's one load, the and,
   the orr and the one store strb w1, [x0]. */
struct flags
{
        int count;
        unsigned dirty : 1, level : 3;
};

void
mark(struct flags *f, int i)
{
        f[i++].dirty = 1;
}

void
raise(struct flags *f, int i, int v)
{
        f[i++].level = v;
}
