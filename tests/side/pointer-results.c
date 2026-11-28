/* Pointer results: a returned pointer in rax and x0 at 64 bits, a call's
   result held there. */
int *g(void);
int *h(int *p, int n);

int *
same(int *p)
{
        return p;
}

int *
null(void)
{
        return 0;
}

int *
null_cast(void)
{
        return (int *) 0;
}

int *
called(void)
{
        return g();
}

int
stored(void)
{
        int *p;
        p = g();
        return p != 0;
}

int
compared(int *p)
{
        return p == g();
}

int *
passed_on(int *p, int n)
{
        return h(h(p, n), n + 1);
}

int **
pointer_to_pointer(int **pp)
{
        return pp;
}

int
tested(void)
{
        if (g())
                return 1;
        return !g() + (g() && h(0, 1));
}
