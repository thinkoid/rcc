/* GNU C's a ?: b, a ? a : b with a evaluated once.  next() returns the
   next value of a counter and counts its calls, so each use of a ?: b
   with next() as a checks both the value and that a was evaluated
   once.  The program checks a 0 and a nonzero a of next(), a signed
   char a converted to int, a pointer a, a register variable assigned
   a ?: b, a ?: b as a condition, a ?: b ?: c, an a whose conversion to
   long is the value, and a ?: b for its effect, whose b runs only when
   a is 0.  It exits 42, or 100 plus the number of the first check that
   failed.

   Under rcc and under clang -O2 it exits 42: nothing here is
   undefined, and clang takes a ?: b as GNU C.

   On x86-64 the listing must show each a ?: b as the value of a in a
   register, cmpl $0 of that register and a jne past b, which is
   written into the same register, and call next@PLT once for each use.

   On aarch64 the listing must show each as the value of a in a
   register, cbnz on it past b, which is written into the same
   register, and bl next once for each use. */
int counter, calls;

int
next(void)
{
        calls++;

        return counter++;
}

int
main(void)
{
        signed char c;
        char *p, *q;
        register int r;
        int n;
        long l;

        if ((next() ?: 7) != 7 || calls != 1)
                return 101;

        if ((next() ?: 7) != 1 || calls != 2)
                return 102;

        c = -3;
        if ((c ?: 9) != -3)
                return 103;

        c = 0;
        if ((c ?: 9) != 9)
                return 104;

        p = 0;
        q = "q";
        if ((p ?: q) != q || (q ?: p) != q)
                return 105;

        r = next() ?: 5;
        if (r != 2 || calls != 3)
                return 106;

        counter = 0;
        if (next() ?: 0)
                return 107;

        counter = 0;
        if ((0 ?: next() ?: 8) != 8 || calls != 5)
                return 108;

        n = -5;
        l = n ?: 3L;
        if (l != -5)
                return 109;

        next() ?: (calls += 10);
        if (calls != 6)
                return 110;

        counter = 0;
        next() ?: (calls += 10);
        if (calls != 17)
                return 111;

        return 42;
}
