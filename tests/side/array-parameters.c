/* An array parameter is a pointer to its element, 6.7.7.4p6, its size
   left out or not: its rows are a pointer's. */
int
unsized(int a[], int i)
{
        return a[i];
}

int
sized(int a[10], int i, int x)
{
        a[i] = x;
        return a[3] + sizeof a;
}

int
pointers(int *p[3], int i)
{
        return *p[i];
}

int caller(int *p);

int
passed(void)
{
        int a[4];
        a[1] = 2;
        return unsized(a, 1) + sized(a, 1, 2) + caller(a);
}
