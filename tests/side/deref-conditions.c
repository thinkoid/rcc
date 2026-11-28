/* *p as a condition: x86-64 compares the object in place, cmpl $0,
   (%rax), testl $1, (%rax), and aarch64 loads it, cbz w0. */
int
conditions(int *p, int x, int **pp)
{
        if (*p)
                x = x + 1;
        if (!*p)
                x = x + 2;
        if (*p == 5)
                x = x + 3;
        if (*p != 0)
                x = x + 4;
        if (x < *p)
                x = x + 5;
        if (*p & 1)
                x = x + 6;
        if (*p + x)
                x = x + 7;
        if (**pp)
                x = x + 8;
        while (*p)
                *p = 0;
        while (*pp)
                *pp = 0;
        return x;
}
