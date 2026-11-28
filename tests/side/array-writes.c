/* An element of a local array written: the slot formed after the value
   where the index is a constant, movl %eax, -8(%rbp) and str w0, [x29,
   #24]; with a variable index the index alone held across the value on
   x86-64, and the array's address with it on aarch64, summed into one
   register before a call. */
int g(void);

int
values(int i, long l, int x)
{
        int a[4];
        a[3] = x;
        a[3] = 7;
        a[3] = 0;
        a[i] = x;
        a[i] = 7;
        a[i] = 0;
        a[l] = x;
        return a[i] = x;
}

int
across(int i, int x, int y, int j)
{
        int a[4];
        int b[4];
        a[i] = g();
        a[2] = g();
        a[i] = x && y;
        a[i] = x / y;
        a[i] = b[j];
        return a[1];
}
