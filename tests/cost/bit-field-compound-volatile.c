/* shape.md chapter 5 and G13, the compound assignment of a bit-field
   whose right operand reads a volatile object: the read is a side
   effect, 5.1.2.4p2, and the right operand is evaluated before the
   object is read as part of the operation, so v is loaded once, into
   a temporary of its own, before the unit is loaded into t.  The rest
   is bit-field-compound's tree, (t = u, u = (((F (t) + v) << s) & M) |
   (t & ~M)), with the temporary in v's place.  f is bits 3 to 7 of
   the byte at 0.

   On x86-64, after the parameter's store, movl v(%rip), %eax; movl
   %eax, -16(%rbp), the operand held, then the pointer's load, movsbl
   (%rax), %eax; movb %al, -9(%rbp) into t, the pointer's second load,
   movsbl -9(%rbp), %ecx; sarl $3, %ecx; addl -16(%rbp), %ecx; shll $3,
   %ecx, then the and with -249, the or and movb %cl, (%rax).

   On aarch64, after the parameter's store, adrp x16, v; ldr w0, [x16,
   :lo12:v]; str w0, [x29, #28], the operand held, then the pointer's
   load, ldrsb w0, [x0]; strb w0, [x29, #24] into t, the pointer's
   second load, ldrsb w1, [x29, #24]; asr w1, w1, #3; ldr w2, [x29,
   #28]; add w1, w1, w2; lsl w1, w1, #3, then the and with 0xffffff07,
   the orr and strb w1, [x0]. */
struct small
{
        unsigned a : 3;
        int f : 5;
};

volatile int v;

void
add(struct small *p)
{
        p->f += v;
}
