/* shape.md chapter 5, a store into a bit-field that does not fill its
   access unit through a place whose address reads a volatile object,
   d->f on a struct small *volatile d: the read of d is a side effect,
   5.1.2.4p2, which the source writes once, so the place's address is
   evaluated once into a temporary and the store names the temporary's
   pointee twice.  d is loaded once; the unit is loaded once and stored
   once.

   On x86-64, movq d(%rip), %rax; movq %rax, -16(%rbp), then movq
   -16(%rbp), %rax for the store, the value's shll $3, movq -16(%rbp),
   %rdx; movsbl (%rdx), %edx, the unit's one load, andl $-249, the or
   and movb %cl, (%rax): two instructions more than the listing through
   a struct small *d, which loads d twice.

   On aarch64, adrp x16, d; ldr x0, [x16, :lo12:d]; str x0, [x29, #24],
   then ldr x0, [x29, #24] for the store, ldr x2, [x29, #24]; ldrsb w2,
   [x2] for the unit's one load, and strb w1, [x0]: one instruction more
   than the listing through a plain pointer. */
struct small
{
        unsigned a : 3;
        int f : 5;
};

struct small *volatile d;

void
store(int v)
{
        d->f = v;
}
