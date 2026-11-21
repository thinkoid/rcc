/* shape.md chapter 5: a local record's list with designators,
   6.7.11p18 to p20, walked as an object of static storage's list is,
   its scalars stores at run time in the list's order (G13): the
   declaration zeroes the whole object, then each value is stored at
   the offset of the subobject the designation names, the walk going
   on from the next one.  member() gives y alone; nested() to.x, to.y
   after it, then from.y; elements() v[2], then tag and the element
   after it, v[0]; called() y the value of f() and then x the value of
   g(); overridden() x twice, f() stored and then g() over it;
   rebraced() from twice, the braced list of the second zeroing from
   again before its value; and switched() a union's i, then its c,
   which zeroes the union again before its store.

   On x86-64 the zeroing is the store of the immediate 0 at each unit
   of the slot, and each value is then stored at its offset: member()
   movl $0, -8(%rbp); movl $0, -4(%rbp), then movl $1, -4(%rbp);
   nested() four movl $0 from -16 to -4, then movl $3, -8(%rbp); movl
   $4, -4(%rbp); movl $2, -12(%rbp); elements() four movl $0, then
   movl $1, -4(%rbp); movb $97, -16(%rbp); movl $2, -12(%rbp); called()
   the two movl $0, then call f@PLT; movl %eax, -4(%rbp) and call
   g@PLT; movl %eax, -8(%rbp); overridden() call f@PLT; movl %eax,
   -8(%rbp) and call g@PLT; movl %eax, -8(%rbp); rebraced() four movl
   $0, movl $1, -16(%rbp); movl $2, -12(%rbp), then movl $0, -16(%rbp);
   movl $0, -12(%rbp) and movl $3, -16(%rbp); and switched() movl $0,
   -4(%rbp), call f@PLT; movl %eax, -4(%rbp), then movl $0, -4(%rbp)
   and movb $120, -4(%rbp).

   On aarch64 the zeroing is the store of the zero register at each
   unit of the slot, and each value is then stored at its offset:
   member() str wzr, [x29, #16]; str wzr, [x29, #20], then mov w0, #1;
   str w0, [x29, #20]; nested() four str wzr from #16 to #28, then the
   3 at #24, the 4 at #28 and the 2 at #20; elements() four str wzr,
   then the 1 at #28, mov w0, #97; strb w0, [x29, #16] and the 2 at
   #20; called() the two str wzr, then bl f; str w0, [x29, #20] and bl
   g; str w0, [x29, #16]; overridden() bl f; str w0, [x29, #16] and bl
   g; str w0, [x29, #16]; rebraced() four str wzr, the 1 at #16 and the
   2 at #20, then str wzr, [x29, #16]; str wzr, [x29, #20] and the 3 at
   #16; and switched() str wzr, [x29, #16], bl f; str w0, [x29, #16],
   then str wzr, [x29, #16] and mov w0, #120; strb w0, [x29, #16]. */
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

union number
{
        int i;
        char c;
};

void use(void *);
int f(void);
int g(void);

void
member(void)
{
        struct point q = { .y = 1 };

        use(&q);
}

void
nested(void)
{
        struct line l = { .to.x = 3, 4, .from.y = 2 };

        use(&l);
}

void
elements(void)
{
        struct named n = { .v[2] = 1, .tag = 'a', 2 };

        use(&n);
}

void
called(void)
{
        struct point q = { .y = f(), .x = g() };

        use(&q);
}

void
overridden(void)
{
        struct point q = { .x = f(), .x = g() };

        use(&q);
}

void
rebraced(void)
{
        struct line l = { .from = { 1, 2 }, .from = { 3 } };

        use(&l);
}

void
switched(void)
{
        union number n = { .i = f(), .c = 'x' };

        use(&n);
}
