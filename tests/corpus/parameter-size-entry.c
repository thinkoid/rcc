/* The size of the array a definition's parameter is adjusted from is
   evaluated on entry to the function, for its effects alone: the
   parameters have block scope (6.2.1p4), and the sizes of their
   declarations are evaluated when the function is entered (6.9.2p11),
   though the parameter is the pointer (6.7.7.4p6).  mark() records the
   number it is called with, in the order of the calls, and returns it;
   each check calls a definition whose sizes call mark() or step a
   parameter, and compares the record and the value it returns with
   what the sizes did.  The checks: a size that steps an earlier
   parameter, one after static and a qualifier, one in parentheses
   around the name, the sizes of two parameters, one of a register
   parameter, one of a static inline definition, and a constant size,
   which does nothing.  It exits 42, or 100 plus the number of the first
   check that failed.

   Under rcc and under clang -O2 it exits 42: the sizes are evaluated
   in the order of the parameters, each once, before the body.

   On x86-64 the listing must show each definition's size evaluated
   after the parameters' stores and before the body, its value
   discarded, and nothing for the constant size.

   On aarch64 the listing must show the same. */
int order[16], marks;

int
mark(int n)
{
        order[marks++] = n;

        return n;
}

/* whether the record holds the numbers 1 to count, in order */
int
in_order(int count)
{
        int i;

        if (marks != count)
                return 0;

        for (i = 0; i < count; i++)
                if (order[i] != i + 1)
                        return 0;

        marks = 0;

        return 1;
}

int
stepped(int n, int a[n++])
{
        return n;
}

int
qualified(int n, int a[static const n++])
{
        return n * 10 + a[0];
}

int
parenthesized(int n, int(a)[n += 2])
{
        return n;
}

int
two(int a[mark(1)], int b[mark(2)])
{
        return mark(3);
}

int
in_register(register int n, int a[n--])
{
        return n;
}

static inline int
inline_size(int n, int a[mark(n)])
{
        return mark(n + 1);
}

int
constant(int n, int a[3])
{
        return n + a[2];
}

int
main(void)
{
        int b[4] = { 7, 0, 5, 0 };

        if (stepped(3, b) != 4)
                return 101;

        if (qualified(3, b) != 47)
                return 102;

        if (parenthesized(3, b) != 5)
                return 103;

        if (two(b, b) != 3 || !in_order(3))
                return 104;

        if (in_register(9, b) != 8)
                return 105;

        if (inline_size(1, b) != 2 || !in_order(2))
                return 106;

        if (constant(1, b) != 6 || marks != 0)
                return 107;

        return 42;
}
