/* *&x is x's place, the builder's, and &*p is a node whose value is
   p's: movq -8(%rbp), %rax, ldr x0, [x29, #16]. */
int
star_amp(void)
{
        int x;
        x = 1;
        *&x = 2;
        return *&x;
}

int *
amp_star(int *p)
{
        return &*p;
}

int
star_amp_star(int *p)
{
        *&*p = 3;
        return *&*p;
}

int
compared(int *p, int *q)
{
        return &*p == q && &*q != 0;
}

int
pointer_place(int **pp)
{
        int *p = *&*pp;
        return &**pp == p;
}
