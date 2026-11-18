/* shape.md chapter 4: a record's assignment, 6.5.17.2, the two lvalues
   of one record, which c1 copies the right's object into the left's
   by: through two pointers, between globals, between a local and a
   parameter's object, between a member of a record type and an
   element of an array of records by a variable index, of a union, and
   in each place a value is not wanted, a statement, a for's clauses,
   the left operand of a comma and the operand of a cast to void.  The
   record's size and alignment are 8 and 4 for point, 24 and 8 for
   wide, 3 and 1 for bytes and 8 and 8 for either.  Beside them the
   copy's sizes and places: codes() a record of 4 bytes at an
   alignment of 2, two units, from an element by an unsigned index
   into a local and out of it into the next element; eight_units() 8
   units of 4 bytes through two pointers; nine_units() 9 of 8, the
   first size a loop copies, nine_named() the same into a local's slot
   and from it into a global, and nine_indexed() between two elements
   by a variable index; text_units() the 32 bytes of a char array, 32
   units of 1, a loop; held() *next() = *last(), the left's place a
   call's result held across the right's call; and registers() *r =
   *q on two register pointers.

   On x86-64 each unit is the load of its width into a scratch
   register, the zero-extending movzbl or movzwl for 1 and 2 bytes, and
   the store, the unit's offset in each place's displacement: through()
   is movl (%rcx), %edx; movl %edx, (%rax) and movl 4(%rcx), %edx;
   movl %edx, 4(%rax) after the two pointers' loads; globals() movl
   corner(%rip), %eax; movl %eax, origin(%rip) and the same at +4, and
   three movq units at second, +8 and +16; local() movzbl (%rax), %ecx;
   movb %cl, -11(%rbp) and two more into the slot, then movzbl
   -11(%rbp), %eax; movb %al, three(%rip) and two more; member() the
   element's operand, movl (%rcx,%rdx,8), %esi; movl %esi, 8(%rax) and
   at 4 and 12, then from l->from to points[i + 1], p's load and i + 1
   first, as written; unions() one movq unit; and effects() each copy
   the same two movl units, in the for's clauses, the cast to void and
   the comma.  codes() is movzwl (%rax,%rcx,4), %edx; movw %dx,
   -16(%rbp) and at 2 and -14, then the same out of the slot into
   (%rax,%rcx,4) after addl $1, %ecx; eight_units() eight movl units
   at 0 to 28; nine_units() movq $-72, %rsi, then the loop movq
   72(%rcx,%rsi,1), %rdx; movq %rdx, 72(%rax,%rsi,1); addq $8, %rsi;
   jne back, with no address formed; nine_named() the loop into
   -8(%rbp,%rdx,1), the slot's index form, then leaq spare(%rip),
   %rax, a label taking no index, and the loop out of the slot;
   nine_indexed() each element's address formed by its imulq $72 and
   addq, since the loop's index takes the form's index, then the loop;
   text_units() movzbl 32(%rcx,%rsi,1), %edx; movb %dl,
   32(%rax,%rsi,1); addq $1, %rsi; jne back; held() next's result
   spilled across the call of last, movq %rax, -8(%rbp), and reloaded,
   then two movzwl and movw units; and registers() movzwl (%r12),
   %eax; movw %ax, (%rbx) and at 2, no copy of either register.

   On aarch64 each unit is the plain load of its width into x17 and its
   store, lowest address first, and no call: through() ldr w17, [x1];
   str w17, [x0], then the same at #4; globals() the label's page and
   access through x16 for each of point's two units, adrp x16, corner;
   ldr w17, [x16, :lo12:corner]; adrp x16, origin; str w17, [x16,
   :lo12:origin], and for wide's three units each label's address formed
   once, adrp x0, first; add x0, x0, :lo12:first, then ldr x17, [x1];
   str x17, [x0] at 0, 8 and 16; local() the three bytes into the slot,
   ldrb w17, [x0]; strb w17, [x29, #24], and out of it to three's
   address formed once; member() the element's index added once, add x1,
   x1, w2, sxtw #3, then ldr w17, [x1]; str w17, [x0, #8], the member's
   8 the store's displacement, and the reverse with the index added to
   the left's place; unions() the one unit of 8 bytes through x16; and
   effects() each copy as a statement's.  codes() adds the index to the
   pointer once, add x0, x0, x1, lsl #2, then ldrh w17, [x0]; strh w17,
   [x29, #28] and at #2 and #30, and the same out of the slot after add
   w1, w1, #1 and the index's add; eight_units() eight ldr w17 and str
   w17 units at #0 to #28; nine_units() the count in x16, mov x16, #9,
   then the loop of four instructions, ldr x17, [x1], #8; str x17, [x0],
   #8; subs x16, x16, #1; b.ne back; nine_named() the slot's address
   formed first, add x1, x29, #24, then the loop into it, and the
   label's adrp and add and the loop out of the slot; nine_indexed()
   each element's address by its mov x2, #72, mul and add, then the
   loop; text_units() the loop with mov x16, #32 and ldrb w17 and strb
   w17 by #1; held() next's result stored to its spill slot across the
   call of last and reloaded once, then two ldrh and strh units; and
   registers() ldrh w17, [x20]; strh w17, [x19] and at #2, the two
   register pointers the bases. */
struct point
{
        int x, y;
};

struct wide
{
        long a;
        char b;
        long c;
};

struct bytes
{
        char a, b, c;
};

struct line
{
        struct point from, to;
};

union either
{
        char c;
        long l;
};

struct point origin, corner;
struct wide first, second;
struct bytes three;
union either one, other;

void
through(struct point *to, struct point *from)
{
        *to = *from;
}

void
globals(void)
{
        origin = corner;
        first = second;
}

int
local(struct bytes *p)
{
        struct bytes b;

        b = *p;
        three = b;

        return b.c;
}

void
member(struct line *l, struct point *points, int i)
{
        l->to = points[i];
        points[i + 1] = l->from;
}

void
unions(void)
{
        one = other;
}

int
effects(struct point *points, int n)
{
        int i;

        for (origin = points[0], i = 0; i < n; corner = points[i], i++)
                ;

        (void) (origin = corner);

        return (corner = origin, corner.x);
}

struct code
{
        unsigned char op, bits;
        unsigned short val;
};

struct eight
{
        int a[8];
};

struct nine
{
        long a[9];
};

struct text
{
        char c[32];
};

struct nine spare;
struct code *next(void);
struct code *last(void);

void
codes(struct code *t, unsigned i)
{
        struct code here;

        here = t[i];
        t[i + 1] = here;
}

void
eight_units(struct eight *to, struct eight *from)
{
        *to = *from;
}

void
nine_units(struct nine *to, struct nine *from)
{
        *to = *from;
}

void
nine_named(struct nine *p)
{
        struct nine t;

        t = *p;
        spare = t;
}

void
nine_indexed(struct nine *to, struct nine *from, int i)
{
        to[i] = from[i];
}

void
text_units(struct text *to, struct text *from)
{
        *to = *from;
}

void
held(void)
{
        *next() = *last();
}

void
registers(struct code *a, struct code *b)
{
        register struct code *r = a, *q = b;

        *r = *q;
}
