/* shape.md chapter 5, a store into a bit-field that does not fill its
   access unit as the middle operand of a comma of three: the operand
   is evaluated for its effect alone, so the store is made and stands
   alone, with no read of the field after it, between the two stores
   of g.  mark() stores 1 into b, the one or on the unit, and set()
   stores r into f.

   On x86-64, mark is movl r(%rip), %eax; movl %eax, g(%rip), then
   orb $1, 1(%rax), then g's store again; set is g's store, then movl
   r(%rip), %ecx; shll $3, %ecx; movsbl (%rdx), %edx; andl $-249, %edx;
   orl %edx, %ecx; movb %cl, (%rax), then g's store again.

   On aarch64, mark is g's store, ldrb w1, [x0, #1]; orr w1, w1, #0x1;
   strb w1, [x0, #1], then g's store again; set is g's store, ldr w1,
   [x16, :lo12:r]; lsl w1, w1, #3; ldrsb w2, [x2]; and w2, w2,
   #0xffffff07; orr w1, w1, w2; strb w1, [x0], then g's store again. */
struct s
{
        unsigned a : 3;
        int f : 5;
        unsigned b : 1;
        unsigned w : 20;
};

int g, r;

void
mark(struct s *p)
{
        g = r, p->b = 1, g = r;
}

void
set(struct s *p)
{
        g = r, p->f = r, g = r;
}
