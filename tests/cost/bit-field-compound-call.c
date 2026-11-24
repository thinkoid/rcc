/* shape.md chapter 5, a compound assignment of a bit-field that does
   not fill its access unit through a call's result, current()->level +=
   v: the place's address is the call's result plus the unit's offset,
   evaluated once into a temporary, so current is called once, before
   the unit is loaded once into t and stored once.

   On x86-64, call current@PLT; addq $4, %rax; movq %rax, -16(%rbp) into
   the temporary, movq -16(%rbp), %rax; movzbl (%rax), %eax; movb %al,
   -5(%rbp) into t, movq -16(%rbp), %rax for the store, the field's read
   of t, addl -4(%rbp), %ecx, the shift, the and, the or and movb %cl,
   (%rax).

   On aarch64, bl current; add x0, x0, #4; str x0, [x29, #24] into the
   temporary, ldr x0, [x29, #24]; ldrb w0, [x0]; strb w0, [x29, #20]
   into t, ldr x0, [x29, #24] for the store, the add of v and strb w1,
   [x0]. */
struct flags
{
        int count;
        unsigned dirty : 1, level : 3;
};

struct flags *current(void);

void
raise(int v)
{
        current()->level += v;
}
