/* Pointer arguments, each stored to its hidden slot of 8 bytes and
   loaded into its register at 64 bits, movq -16(%rbp), %rdi and ldr x0,
   [x29, #24]; x86-64 pushes a seventh from its slot, pushq -64(%rbp),
   and aarch64 copies a ninth to the outgoing area, ldr x17 and str x17,
   [sp]. */
int g(int *p);
int g7(int *a, int *b, int *c, int *d, int *e, int *f, int *h);
int g9(int *a, int *b, int *c, int *d, int *e, int *f, int *h, int *i, int *j);
int gm(int n, int *a, char c, int *b, long l, int *d, short s, int *e, int *f);

int
passed(int *p)
{
        return g(p);
}

int
null(void)
{
        return g(0) + g((int *) 0);
}

int
address(void)
{
        int x = 1;
        return g(&x);
}

int
seven(int *p)
{
        return g7(p, p, p, p, p, p, p);
}

int
nine(int *p)
{
        int x;
        return g9(p, p, &x, p, p, p, p, p, &x);
}

int
mixed(int *p)
{
        int x = 3;
        return gm(1, p, 'c', &x, 5, 0, 7, p, &x);
}
