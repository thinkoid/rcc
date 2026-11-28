/* An element of a local array read: a constant index folded into the
   slot, -4(%rbp) and [x29, #28]; a variable one the slot and the index
   on x86-64, -20(%rbp,%rax,4), and the array's address and the index on
   aarch64, add x0, x29, #20 and [x0, w1, sxtw #2], held across a
   call. */
int g(void);

int
constants(void)
{
        int a[4];
        a[0] = 1;
        return a[3] + a[0] + *a + a[-1] + a[4] + *(a + 2) + *(a + 3 - 1);
}

int
indexes(int i, long l, unsigned u)
{
        int a[4];
        a[0] = 1;
        return a[i] + *(a + i) + i[a] + a[l] + a[u] + a[i + 1];
}

int
across(int i)
{
        int a[4];
        a[0] = 1;
        return a[g()] + *(a - i) + a[i] * a[i / 2];
}

int
pointers(int i)
{
        int x = 3;
        int *p[3];
        p[0] = &x;
        p[i] = &x;
        *p[0] = 4;
        return *p[i] + **p;
}

/* *&a[i] is the place through &a[i], as in rcc's tree: aarch64 adds the
   index to x29 and reads [x0, #16], where a[i] is [x0, w1, sxtw #2]. */
int
through_address(int i, int j)
{
        int a[4];
        a[0] = 1;
        *&a[j] = 2;
        return *&a[i] + *&a[3] + *&*(a + i);
}
