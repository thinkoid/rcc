/* shape.md chapter 5, the compound assignment and the step of a
   bit-field that does not fill its access unit, the parser's tree of
   one temporary t of the unit's type, by R bits, 32 for a unit of 1, 2
   or 4 bytes: (t = u, u = (((F (t) op v) << s) & M) | (t & ~M)), F
   (t) the field's read of t, s the field's bit and M its mask in the
   unit, with no & M where the field reaches its unit's top, as high
   does.  The unit is loaded once, into t, and stored once; t is read
   twice, for the field's value and for the bits kept; the place is
   written twice, its pointer loaded twice.  A step is += 1 or -= 1.
   A _Bool field's result is compared with 0 before it is shifted in.
   A right operand with a side effect, tick (), is held in a temporary
   of its own before the unit is loaded.  Where the value is used,
   a prefix step's and a compound assignment's is the field's read of
   the unit stored, with no second load of the unit, and a postfix
   step's is F (t); as a statement each is the store alone, so step
   and stepped are one listing.  counter's low is bits 0 to 3 and high
   bits 4 to 7 of the byte at 0, level bits 0 to 4 and on bit 5 of the
   byte at 1.

   On x86-64, bump is the pointer's load, movzbl (%rax), %eax; movb
   %al, -9(%rbp) into t, the pointer's second load, then movzbl
   -9(%rbp), %ecx; shrl $4, %ecx; addl $2, %ecx; shll $4, %ecx; movzbl
   -9(%rbp), %edx; andl $-241, %edx; orl %edx, %ecx; movb %cl, (%rax),
   12 instructions.  raised is bump's sequence into eax, then movzbl
   %al, %eax; shrl $4, %eax.  step and stepped read low by shll $28 and
   shrl $28, then addl $1, %ecx; andl $15, %ecx.  before is stepped's,
   then movzbl -9(%rbp), %eax; shll $28, %eax; shrl $28, %eax, t's read.
   lowered is movsbl 1(%rax), %eax into t, shll $27 and sarl $27, subl
   $1, andl $31, the and with -32 and the or, movb %al, 1(%rcx), then
   movsbl %al, %eax; shll $27, %eax; sarl $27, %eax.  toggle reads on by
   shll $26 and shrl $31, then xorl $1, %ecx; cmpl $0, %ecx; setne %cl;
   movzbl %cl, %ecx; shll $5, %ecx; andl $32, %ecx.  turned is subl $1
   in its place, then t's read.  ticked is call tick@PLT; movl %eax,
   -16(%rbp) before the unit's load, then the update of level with addl
   -16(%rbp), %ecx for the operation.

   On aarch64, bump is ldr x0, [x29, #16]; ldrb w0, [x0]; strb w0,
   [x29, #24], the pointer's second load, then ldrb w1, [x29, #24]; lsr
   w1, w1, #4; add w1, w1, #2; lsl w1, w1, #4; ldrb w2, [x29, #24];
   and w2, w2, #0xffffff0f; orr w1, w1, w2; strb w1, [x0], 12
   instructions.  raised is bump's, then uxtb w1, w1; lsr w0, w1, #4.
   lowered loads by ldrsb and reads by lsl and asr, then sxtb w1, w1;
   lsl w1, w1, #27; asr w0, w1, #27.  toggle is eor w1, w1, #0x1; cmp
   w1, #0; cset w1, ne, then lsl w1, w1, #5 and the rest.  ticked is bl
   tick; str w0, [x29, #28] before the unit's load, and ldr w2, [x29,
   #28]; add w1, w1, w2 for the operation. */
struct counter
{
        unsigned low : 4, high : 4;
        int level : 5;
        _Bool on : 1;
};

int tick(void);

void
bump(struct counter *p)
{
        p->high += 2;
}

int
raised(struct counter *p)
{
        return p->high += 2;
}

void
step(struct counter *p)
{
        ++p->low;
}

void
stepped(struct counter *p)
{
        p->low++;
}

int
before(struct counter *p)
{
        return p->low++;
}

int
lowered(struct counter *p)
{
        return --p->level;
}

void
toggle(struct counter *p)
{
        p->on ^= 1;
}

int
turned(struct counter *p)
{
        return p->on--;
}

void
ticked(struct counter *p)
{
        p->level += tick();
}
