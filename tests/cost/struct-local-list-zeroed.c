/* shape.md chapter 5: the zeroing of a local record initialized by a
   list, the whole object, padding included, from the lowest address up
   in units of the record's alignment, at most 8 bytes, as a record's
   copy goes (G18), before the first listed value is stored.  The
   records are of 8 bytes at an alignment of 4, two units; of 24 at 8,
   three; of 200 at 8, twenty-five, more than eight; of 12 at 4 with
   padding after each char; and a union of 4 at 2 whose first member
   holds 3.  point_fewer(), wide_fewer() and large_fewer() list fewer
   values than members, padded_full() every member, point_empty() none,
   and union_shorter() the union's first member, which leaves its last
   byte.

   On x86-64 each unit is the store of the immediate 0 at its width
   into the slot, lowest address first, and takes no register:
   point_fewer() movl $0, -12(%rbp); movl $0, -8(%rbp), then x's load
   and its store at -12; wide_fewer() movq $0 at -32, -24 and -16,
   then a and a + 1 stored at -32 and -24; large_fewer() the loop of
   one index, movq $-200, %rax, then movq $0, (%rbp,%rax,1); addq $8,
   %rax; jne back, four instructions, and movq $1, -200(%rbp); movq $2,
   -192(%rbp) after it; padded_full() three movl $0 at -20 to -12, the
   padding after each char among them, then the members' stores, movb
   %al, -20(%rbp), movl %eax, -16(%rbp) and movb %al, -12(%rbp);
   point_empty() the two movl $0 alone; and union_shorter() movw $0,
   -4(%rbp); movw $0, -2(%rbp), then movb $97, -4(%rbp), the rest of
   the union left zero.

   On aarch64 each unit is the store of the zero register at its
   width into the slot, lowest address first, and takes no other
   register: point_fewer() str wzr, [x29, #20]; str wzr, [x29, #24],
   then x's load and its store at #20; wide_fewer() str xzr at #24,
   #32 and #40, then a and a + 1 stored at #24 and #32; large_fewer()
   the slot's address and the count first, add x0, x29, #16; mov x16,
   #25, then the loop of three instructions, str xzr, [x0], #8; subs
   x16, x16, #1; b.ne back, and mov x0, #1; str x0, [x29, #16] and the
   2 at #24 after it; padded_full() three str wzr at #24 to #32, the
   padding after each char among them, then the members' stores, strb
   w0, [x29, #24], str w0, [x29, #28] and strb w0, [x29, #32];
   point_empty() the two str wzr alone; and union_shorter() strh wzr,
   [x29, #16]; strh wzr, [x29, #18], then mov w0, #97; strb w0, [x29,
   #16], the rest of the union left zero. */
struct point
{
        int x, y;
};

struct wide
{
        long a, b, c;
};

struct large
{
        long a[25];
};

struct padded
{
        char c;
        int i;
        char d;
};

union either
{
        char c[3];
        short s;
};

void use(void *);

void
point_fewer(int x)
{
        struct point q = { x };

        use(&q);
}

void
wide_fewer(long a)
{
        struct wide w = { a, a + 1 };

        use(&w);
}

void
large_fewer(void)
{
        struct large l = { 1, 2 };

        use(&l);
}

void
padded_full(char c, int i)
{
        struct padded p = { c, i, c };

        use(&p);
}

void
point_empty(void)
{
        struct point q = { };

        use(&q);
}

void
union_shorter(void)
{
        union either e = { { 'a' } };

        use(&e);
}
