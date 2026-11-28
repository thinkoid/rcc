/* The frame with arrays among the locals: each array's slot its size
   at its element's alignment, between the others' in the order
   declared; an array parameter a pointer's slot. */
int
mixed(void)
{
        int x = 1;
        int a[3];
        int *p = &x;
        int y = 2;
        a[0] = x;
        return a[0] + y + *p;
}

int
after(void)
{
        int a[3];
        int x = 1;
        a[2] = x;
        return a[2];
}

int
pointers(void)
{
        int x;
        int *p[3];
        p[0] = &x;
        *p[0] = 4;
        return x;
}

int
large(int i)
{
        int a[33];
        int b[100];
        a[0] = 1;
        b[99] = i;
        return a[0] + b[i] + b[99];
}

int
parameters(int x, int *p, int a[], int y)
{
        return x + *p + a[0] + y;
}

int
order(int a, int b)
{
        int *p = &a;
        int c[2];
        int *q = &b;
        c[1] = *p;
        return c[1] + *q;
}
