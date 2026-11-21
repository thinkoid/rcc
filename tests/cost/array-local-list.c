/* shape.md chapter 5: a local array initialized by a list, 6.7.11p17
   to p23, as a local record's list is: the declaration zeroes the
   whole array from the lowest address up, in units of the array's
   alignment, its element's, at most 8 bytes, as a record's zeroing
   goes (G20), then each listed value is stored at its element's offset
   in the slot, in the list's order (G13).  fewer() gives two of three
   ints; records() two points, the second's x by elision; counted() an
   array of unknown size two longs, which its list completes, p23;
   designated() the values of two calls by designators, each stored
   before the next call; bytes() two of five chars, in units of 1;
   large() one of 25 longs, past eight units; and byte_loop() one of
   20 chars, past eight units of 1.

   On x86-64 each unit of the zeroing is the store of the immediate 0
   at its width, and each value is then stored at its offset: fewer()
   movl $0 at -12, -8 and -4, then movl $1, -12(%rbp); movl $2,
   -8(%rbp); records() four movl $0 from -16 to -4, then the 1, the 2
   and the 3 at -16, -12 and -8; counted() movq $0 at -16 and -8, then
   movq $1, -16(%rbp); movq $2, -8(%rbp); designated() four movl $0,
   then call f@PLT; movl %eax, -4(%rbp) and call g@PLT; movl %eax,
   -12(%rbp); bytes() movb $0 at -5 to -1, then movb $97, -5(%rbp);
   movb $98, -4(%rbp); large() the loop of one index, movq $-200, %rax,
   then movq $0, (%rbp,%rax,1); addq $8, %rax; jne back, and movq $1,
   -200(%rbp) after it; and byte_loop() the same loop by bytes, movq
   $-20, %rax, then movb $0, (%rbp,%rax,1); addq $1, %rax; jne back,
   and movb $120, -20(%rbp).

   On aarch64 each unit of the zeroing is the store of the zero
   register at its width, and each value is then stored at its offset:
   fewer() str wzr at #16, #20 and #24, then mov w0, #1; str w0, [x29,
   #16] and the 2 at #20; records() four str wzr from #16 to #28, then
   the 1, the 2 and the 3 at #16, #20 and #24; counted() str xzr at #16
   and #24, then mov x0, #1; str x0, [x29, #16] and the 2 at #24;
   designated() four str wzr, then bl f; str w0, [x29, #28] and bl g;
   str w0, [x29, #20]; bytes() strb wzr at #16 to #20, then mov w0,
   #97; strb w0, [x29, #16] and the 98 at #17; large() add x0, x29,
   #16; mov x16, #25, then str xzr, [x0], #8; subs x16, x16, #1; b.ne
   back, and mov x0, #1; str x0, [x29, #16] after it; and byte_loop()
   add x0, x29, #16; mov x16, #20, then strb wzr, [x0], #1; subs x16,
   x16, #1; b.ne back, and mov w0, #120; strb w0, [x29, #16]. */
struct point
{
        int x, y;
};

void use(void *);
int f(void);
int g(void);

void
fewer(void)
{
        int a[3] = { 1, 2 };

        use(a);
}

void
records(void)
{
        struct point a[2] = { { 1, 2 }, 3 };

        use(a);
}

void
counted(void)
{
        long a[] = { 1, 2 };

        use(a);
}

void
designated(void)
{
        int a[4] = { [3] = f(), [1] = g() };

        use(a);
}

void
bytes(void)
{
        char a[5] = { 'a', 'b' };

        use(a);
}

void
large(void)
{
        long a[25] = { 1 };

        use(a);
}

void
byte_loop(void)
{
        char a[20] = { 'x' };

        use(a);
}
