/* shape.md chapter 5, a postfix step of a bit-field that does not fill
   its access unit through a stepped pointer, cursor++->level++, its
   value used: the place's address, the stepped pointer's old value plus
   the unit's offset, is evaluated once into a temporary, so cursor is
   stepped once; the unit is loaded once into t and stored once, and the
   value is the field's read of t, the value before.

   On x86-64, movq cursor(%rip), %rax; movq %rax, %rcx; addq $8, %rax;
   movq %rax, cursor(%rip), then addq $4, %rcx; movq %rcx, -16(%rbp)
   into the temporary, movq -16(%rbp), %rax; movzbl (%rax), %eax; movb
   %al, -1(%rbp) into t, movq -16(%rbp), %rax for the store, addl $1,
   movb %cl, (%rax), and the value movzbl -1(%rbp), %eax; shll $28,
   %eax; shrl $29, %eax.

   On aarch64, the step of cursor, add x0, x0, #4; str x0, [x29, #24]
   into the temporary, ldr x0, [x29, #24]; ldrb w0, [x0]; strb w0, [x29,
   #16] into t, ldr x0, [x29, #24] for the store, add w1, w1, #1, strb
   w1, [x0], and the value ldrb w0, [x29, #16]; lsl w0, w0, #28; lsr w0,
   w0, #29. */
struct flags
{
        int count;
        unsigned dirty : 1, level : 3;
};

struct flags *cursor;

int
next(void)
{
        return cursor++->level++;
}
