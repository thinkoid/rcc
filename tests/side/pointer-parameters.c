/* Pointer parameters: each a slot of 8 bytes stored from its register,
   movq %rdi, -8(%rbp) and str x0, [x29, #16]; on x86-64 the seventh and
   later are read where the caller pushed them, 16(%rbp) and up, and on
   aarch64 the ninth and later are copied into their slots through x17.
   */
int
compared(int *p, int *q)
{
        return (p == q) + (p < q) + !p + (p != 0);
}

int
seven(int *a, int *b, int *c, int *d, int *e, int *f, int *g)
{
        return a == g;
}

int
nine(int *a, int *b, int *c, int *d, int *e, int *f, int *g, int *h, int *i)
{
        return g == i || h != 0;
}

int
mixed(int n, int *a, int m, int *b, int k, int *c, int j, int *d, int *e)
{
        return (a == d) + (e < c) + n + m + k + j + (b != 0);
}

int
tested(int *p, int *q)
{
        if (p)
                return 1;
        while (q)
                q = 0;
        return p && q;
}

int
address_of_parameters(int a, int *p)
{
        int *q = &a;
        int **pp = &p;
        return q == p || pp == 0;
}

long
cast(int *p)
{
        return (long) p + (int) p + (long) (int **) p;
}
