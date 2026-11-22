/* shape.md chapter 5, a bit-field as a condition: its read, the
   parser's shifts of its unit, then the condition c1 writes on that
   value, its comparison with 0 at its width and the jump, or on
   aarch64 cbz and cbnz; ! and == 0 are the same test with the jump the
   other way.  job's used is bit 3 of the byte at 7 and waited bit 2,
   one-bit unsigned fields, and mode's level a signed 4-bit field at
   bit 3 of the byte at 0.

   On x86-64: if_used is movzbl 7(%rax), %eax; shll $28, %eax; shrl
   $31, %eax; cmpl $0, %eax; je past the call, and if_not_used and
   if_used_zero the same with jne; used_and_waited is used's read and
   je, then waited's with shll $29 and jne, to the 1 and the 0;
   if_level is movsbl (%rax), %eax; shll $25, %eax; sarl $28, %eax;
   cmpl $0, %eax; je, and if_not_level and if_level_zero with jne.

   On aarch64: the same loads and shifts, ldrb w0, [x0, #7]; lsl w0,
   w0, #28; lsr w0, w0, #31, then cbz for if_used, cbnz for
   if_not_used, and cmp w0, #0; b.ne for if_used_zero; ldrsb, lsl #25
   and asr #28 for level, with cbz, cbnz and cmp and b.ne alike. */
struct job
{
        int stop;
        unsigned nprocs : 16, state : 8, sigint : 1, jobctl : 1, waited : 1,
            used : 1, changed : 1;
        struct job *prev;
};

struct mode
{
        int kind : 3;
        int level : 4;
};

void f(void);

void
if_used(struct job *p)
{
        if (p->used)
                f();
}

void
if_not_used(struct job *p)
{
        if (!p->used)
                f();
}

void
if_used_zero(struct job *p)
{
        if (p->used == 0)
                f();
}

int
used_and_waited(struct job *p)
{
        return p->used && !p->waited;
}

void
if_level(struct mode *p)
{
        if (p->level)
                f();
}

void
if_not_level(struct mode *p)
{
        if (!p->level)
                f();
}

void
if_level_zero(struct mode *p)
{
        if (p->level == 0)
                f();
}
