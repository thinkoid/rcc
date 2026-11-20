/* shape.md chapter 5: a local record initialized by a list, 6.7.11p17
   to p22, walked as an object of static storage's list is, its
   scalars stores at run time: the declaration zeroes the whole object,
   then each listed value is converted to its scalar's type and stored
   at the scalar's offset in the slot, in the list's order (G13).
   full() gives a point both its members; nested() a line, its first
   point by elision and its second in braces of its own; arrays() a
   record with a member array, braced and elided; first() a union's
   first member; called() the values of two calls, each stored before
   the next call; and counted() a point in a for's first clause.

   On x86-64 the zeroing is struct-local-list-zeroed's, the store of
   the immediate 0 at each unit of the slot, and each value is then
   stored at its offset, a constant by the one store of the immediate:
   full() movl $0, -8(%rbp); movl $0, -4(%rbp), then movl $1,
   -8(%rbp); movl $2, -4(%rbp); nested() four movl $0 from -16 to -4,
   then the four values there, the first point's by elision; arrays()
   four movl $0 into each record of 16 bytes, then for n movb $97,
   -16(%rbp), movl $1, -12(%rbp) and movl $2, -8(%rbp), its third
   element left zero, and for m movb $98, -32(%rbp) and its three
   elements at -28 to -20; first() movw $0, -4(%rbp); movw $0,
   -2(%rbp), then movb $120, -4(%rbp) and the two others at -3 and -2;
   called() the two movl $0, then call f@PLT; movl %eax, -8(%rbp) and
   call g@PLT; movl %eax, -4(%rbp); and counted() movl $0, -12(%rbp);
   movl $0, -8(%rbp), then movl $0, -12(%rbp) and movl $3, -8(%rbp),
   all before the loop's head.

   On aarch64 the zeroing is struct-local-list-zeroed's, the store of
   the zero register at each unit of the slot, and each value is then
   stored at its offset, a constant through a register and a zero from
   the zero register: full() str wzr, [x29, #16]; str wzr, [x29, #20],
   then mov w0, #1; str w0, [x29, #16] and the 2 at #20; nested() four
   str wzr from #16 to #28, then the four values there; arrays() four
   str wzr into each record of 16 bytes, then for n mov w0, #97; strb
   w0, [x29, #16] and the 1 and the 2 at #20 and #24, its third element
   left zero, and for m the 98 at #32 and its three elements at #36 to
   #44; first() strh wzr, [x29, #16]; strh wzr, [x29, #18], then mov
   w0, #120; strb w0, [x29, #16] and the two others at #17 and #18;
   called() the two str wzr, then bl f; str w0, [x29, #16] and bl g;
   str w0, [x29, #20]; and counted() str wzr, [x29, #20]; str wzr,
   [x29, #24] after n's, then str wzr, [x29, #20] and mov w0, #3; str
   w0, [x29, #24], all before the loop's head. */
struct point
{
        int x, y;
};

struct line
{
        struct point from, to;
};

struct named
{
        char tag;
        int v[3];
};

union either
{
        char c[3];
        short s;
};

void use(void *);
int f(void);
int g(void);

void
full(void)
{
        struct point q = { 1, 2 };

        use(&q);
}

void
nested(void)
{
        struct line l = { 1, 2, { 3, 4 } };

        use(&l);
}

void
arrays(void)
{
        struct named n = { 'a', { 1, 2 } };
        struct named m = {
                'b',
                3,
                4,
                5,
        };

        use(&n);
        use(&m);
}

void
first(void)
{
        union either e = { { 'x', 'y', 'z' } };

        use(&e);
}

void
called(void)
{
        struct point q = { f(), g() };

        use(&q);
}

int
counted(void)
{
        int n = 0;

        for (struct point i = { 0, 3 }; i.x < i.y; i.x++)
                n += i.x;

        return n;
}
