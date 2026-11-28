/* A local array as a value, its decay: its slot's address, leaq
   -16(%rbp), %rax and add x0, x29, #16, a constant folded into the
   slot, leaq -4(%rbp), %rax for a + 3, and an index the indexed form's
   leaq on x86-64; &a is the same address, of a pointer to the array. */
int g(int *p);

int *
decay(void)
{
        int a[4];
        return a;
}

int *
sums(int i, long l)
{
        int a[4];
        int *p = a;
        p = &a[0];
        p = &a[3];
        p = a + 3;
        p = a - 1;
        p = &a[i];
        p = a + i;
        p = i + a;
        p = &a[l];
        return a - i;
}

long
compared(int *p)
{
        int a[4];
        a[0] = g(a);
        if (a)
                return &a[3] - a;
        return (p == a) + (a < p) + !a + (&a == 0) + (&a != &a);
}
