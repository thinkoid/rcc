/* shape.md chapter 5, the read of a bit-field that does not fill its
   access unit: the unit's load, of the unit's width and the field's
   signedness, extended to R bits, 32 for a unit of 1, 2 or 4 bytes and
   64 for one of 8, then the shifts that bring the field's bits to the
   bottom, by the parser's tree and with no mask.  A field that reaches
   its unit's top is the load and one right shift by its bit; any other
   field is a left shift by R less its bit and its width and a right
   shift by R less its width.  The right shift is arithmetic for a
   signed field and logical for an unsigned one.  Each record holds a
   field at bit 0, one in the middle and one at the top, of a byte unit
   in sbyte and ubyte, of a 4-byte unit in sword and uword, each in an
   int container of its own, and of an 8-byte unit in slong and ulong;
   flags holds two _Bool fields, of which b is at bit 1.  Each function
   returns its field in the field's promoted type, so nothing converts
   the value after its read.

   On x86-64: sbyte_lo is movsbl (%rax), %eax; shll $29, %eax; sarl
   $29, %eax, sbyte_mid shll $26 and sarl $29, and sbyte_top sarl $6
   alone; ubyte's are the same with movzbl and shrl.  sword_lo is movl
   (%rax), %eax; shll $12; sarl $12, sword_mid movl 4(%rax) with shll
   $11 and sarl $23, and sword_top movl 8(%rax) with sarl $15 alone;
   uword's with shrl.  slong_lo is movq (%rax), %rax; shlq $24; sarq
   $24, slong_mid movq 8(%rax) with shlq $14 and sarq $34, and
   slong_top movq 16(%rax) with sarq $20 alone; ulong's with shrq.
   flags_b is movzbl (%rax), %eax; shll $30, %eax; shrl $31, %eax.

   On aarch64 the same shifts: ldrsb, ldrb, ldr w and ldr x for the
   loads, ldr w0, [x0, #4] for sword_mid, then lsl and asr or lsr, of
   w registers for the units of 1 to 4 bytes and of x registers for
   those of 8. */
struct sbyte
{
        int lo : 3;
        int mid : 3;
        int top : 2;
};

struct ubyte
{
        unsigned lo : 3;
        unsigned mid : 3;
        unsigned top : 2;
};

struct sword
{
        int lo : 20;
        int : 0;
        int a : 12;
        int mid : 9;
        int : 0;
        int b : 15;
        int top : 17;
};

struct uword
{
        unsigned lo : 20;
        unsigned : 0;
        unsigned a : 12;
        unsigned mid : 9;
        unsigned : 0;
        unsigned b : 15;
        unsigned top : 17;
};

struct slong
{
        long lo : 40;
        long : 0;
        long a : 20;
        long mid : 30;
        long : 0;
        long b : 20;
        long top : 44;
};

struct ulong
{
        unsigned long lo : 40;
        unsigned long : 0;
        unsigned long a : 20;
        unsigned long mid : 30;
        unsigned long : 0;
        unsigned long b : 20;
        unsigned long top : 44;
};

struct flags
{
        _Bool a : 1, b : 1;
};

int
sbyte_lo(struct sbyte *p)
{
        return p->lo;
}

int
sbyte_mid(struct sbyte *p)
{
        return p->mid;
}

int
sbyte_top(struct sbyte *p)
{
        return p->top;
}

int
ubyte_lo(struct ubyte *p)
{
        return p->lo;
}

int
ubyte_mid(struct ubyte *p)
{
        return p->mid;
}

int
ubyte_top(struct ubyte *p)
{
        return p->top;
}

int
sword_lo(struct sword *p)
{
        return p->lo;
}

int
sword_mid(struct sword *p)
{
        return p->mid;
}

int
sword_top(struct sword *p)
{
        return p->top;
}

int
uword_lo(struct uword *p)
{
        return p->lo;
}

int
uword_mid(struct uword *p)
{
        return p->mid;
}

int
uword_top(struct uword *p)
{
        return p->top;
}

long
slong_lo(struct slong *p)
{
        return p->lo;
}

int
slong_mid(struct slong *p)
{
        return p->mid;
}

long
slong_top(struct slong *p)
{
        return p->top;
}

unsigned long
ulong_lo(struct ulong *p)
{
        return p->lo;
}

int
ulong_mid(struct ulong *p)
{
        return p->mid;
}

unsigned long
ulong_top(struct ulong *p)
{
        return p->top;
}

int
flags_b(struct flags *p)
{
        return p->b;
}
