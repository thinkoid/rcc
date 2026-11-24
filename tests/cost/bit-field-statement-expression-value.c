/* shape.md chapter 5, a store into a bit-field that does not fill its
   access unit as the last statement of a statement expression: the
   statement expression's value is the store's, the field's read of the
   unit the store leaves, so the store is followed by the read's
   shifts, as for return p->f = v;.

   On x86-64, the unit's store movb %al, (%rcx), then movsbl %al, %eax;
   sarl $3, %eax, the field's value.

   On aarch64, the unit's store strb w1, [x0], then sxtb w1, w1; asr
   w0, w1, #3, the field's value. */
struct s
{
        unsigned a : 3;
        int f : 5;
        unsigned b : 1;
        unsigned w : 20;
};

int
store(struct s *p, int v)
{
        return ({ p->f = v; });
}
