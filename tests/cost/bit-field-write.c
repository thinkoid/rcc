/* shape.md chapter 5, the store into a bit-field that does not fill
   its access unit, the parser's tree, by R bits, 32 for a unit of 1, 2
   or 4 bytes.  A constant that is 0 in the field's bits is u &= ~M, M
   the field's mask in the unit, and one that is all ones u |= M, one
   compound assignment on the unit, the place written once; so is the
   store of 1 through jobs[i++], whose place has a side effect.  Any
   other value v is u = ((v << s) & M) | (u & ~M), s the field's bit,
   the value first, then the unit's one load, its and with the
   complement of the mask, the or and the one store; the place is
   written twice, its pointer loaded twice.  Where the field reaches
   its unit's top, as f and b do, the value is v << s with no and: the
   shift clears the bits below the field, and the store of the byte
   drops those above it.  A constant's placed bits are one constant.  A
   _Bool field's value is its comparison with 0.  The value of a store,
   in stored(), is the field's read of the value stored, with no second
   load of the unit.  job's used is bit 3 and changed bit 4 of the byte
   at 7; small's f is bits 3 to 7 of the byte at 0, u bits 0 to 6 and b
   bit 7 of the byte at 1.

   On x86-64: set_used is orb $8, 7(%rax), clear_changed andb $-17,
   7(%rax), clear_f andb $7, (%rax), fill_f orb $-8, (%rax), clear_u
   andb $-128, 1(%rax) and fill_u orb $127, 1(%rax), each after the
   pointer's one load.  five is the pointer's load, its second load
   movq -8(%rbp), %rcx, then movsbl (%rcx), %ecx; andl $-249, %ecx;
   orl $40, %ecx; movb %cl, (%rax).  store is the pointer's load, then
   movl -12(%rbp), %ecx; shll $3, %ecx, the pointer's second load movq
   -8(%rbp), %rdx, then movsbl (%rdx), %edx; andl $-249, %edx; orl %edx,
   %ecx; movb %cl, (%rax).  store_bool is cmpl $0, -12(%rbp); setne %cl;
   movzbl %cl, %ecx; shll $7, %ecx, then the second load and movzbl
   1(%rdx), %edx; andl $-129, %edx; orl; movb %cl, 1(%rax).  stored is
   store's sequence into eax, then movsbl %al, %eax; sarl $3, %eax.
   mark steps i, then orb $8, 7(%rax,%rdx,1) after the index's scaling.

   On aarch64: the constant cases are the unit's load, and or orr of
   the mask and the store, ldrb w1, [x0, #7]; orr w1, w1, #0x8; strb
   w1, [x0, #7] for set_used and and w1, w1, #0xffffffef for
   clear_changed; five is the second load ldr x1, [x29, #16], then
   ldrsb w1, [x1]; and w1, w1, #0xffffff07; mov w2, #40; orr w1, w1,
   w2; strb w1, [x0]; store is ldr w1, [x29, #24]; lsl w1, w1, #3, the
   second load ldr x2, [x29, #16], then ldrsb w2, [x2]; and w2, w2,
   #0xffffff07; orr w1, w1, w2; strb w1, [x0]; store_bool cmp w1, #0;
   cset w1, ne, then the same shape at bit 7; stored store's sequence,
   then sxtb w1, w1; asr w0, w1, #3; and mark steps i, then add x0, x0,
   w1, sxtw #4 and the load, orr and store of the byte at 7. */
struct job
{
        int stop;
        unsigned nprocs : 16, state : 8, sigint : 1, jobctl : 1, waited : 1,
            used : 1, changed : 1;
        struct job *prev;
};

struct small
{
        unsigned a : 3;
        int f : 5;
        unsigned u : 7;
        _Bool b : 1;
};

void
set_used(struct job *p)
{
        p->used = 1;
}

void
clear_changed(struct job *p)
{
        p->changed = 0;
}

void
clear_f(struct small *p)
{
        p->f = 0;
}

void
fill_f(struct small *p)
{
        p->f = -1;
}

void
clear_u(struct small *p)
{
        p->u = 0;
}

void
fill_u(struct small *p)
{
        p->u = 127;
}

void
five(struct small *p)
{
        p->f = 5;
}

void
store(struct small *p, int v)
{
        p->f = v;
}

void
store_bool(struct small *p, int v)
{
        p->b = v;
}

int
stored(struct small *p, int v)
{
        return p->f = v;
}

void
mark(struct job *jobs, int i)
{
        jobs[i++].used = 1;
}
