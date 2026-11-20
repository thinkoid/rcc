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
   all before the loop's head. */
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
