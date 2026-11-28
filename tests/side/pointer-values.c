/* Pointers as values: the address of a local, leaq -4(%rbp), %rax and
   add x0, x29, #16, stored in a pointer local's slot of 8 bytes,
   compared at 64 bits, unsigned, and tested as a condition. */
int
address_compared(void)
{
        int x = 1;
        int y = 2;
        return &x == &y;
}

int
address_stored(void)
{
        int x = 1;
        int *p = &x;
        int *q;
        q = p;
        p = &x;
        return p == q;
}

int
address_against_local(void)
{
        int x = 1;
        int *p = &x;
        return (&x == p) + (p == &x) + (p != &x);
}

int
relations(void)
{
        int x = 1;
        int y = 2;
        int *p = &x;
        int *q = &y;
        return (p < q) + (p <= q) + (p > q) + (p >= q) + (p != q);
}

int
against_null(void)
{
        int x = 1;
        int *p = &x;
        return (p == 0) + (0 == p) + (p != 0) + (0 != p);
}

int
not_and_or(void)
{
        int x = 1;
        int *p = &x;
        int *q = 0;
        return !p + (p && q) + (p || q) + !(p && x);
}

int
conditions(void)
{
        int x = 3;
        int *p = &x;
        int *q = 0;
        if (p)
                x = x + 1;
        while (q)
                q = 0;
        if (!p || q)
                return 1;
        for (; p != q; p = q)
                x = x + 2;
        return x;
}

int
address_of_parameter(
    int a, int b, int c, int d, int e, int f, int g, int h, int i)
{
        int *p = &a;
        int *q = &g;
        int *r = &i;
        return (p == q) + (q == r);
}

int
pointer_to_pointer(void)
{
        int x = 1;
        int *p = &x;
        int **pp = &p;
        int **qq = pp;
        return pp == qq && pp != 0;
}

int
address_discarded(void)
{
        int x = 1;
        &x;
        return x;
}
