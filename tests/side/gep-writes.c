/* The subscript written: the place formed first and held across the
   value, summed into one register before a value that spills, leaq
   (%rax,%rcx,4), %rax and add x0, x0, w1, sxtw #2, a constant index
   past reach held back on aarch64, mov x1, #5000 after the call. */
int g(void);

int
values(int *p, int i, int x)
{
        p[i] = x;
        p[3] = 7;
        p[i] = 0;
        i[p] = x;
        *(p + i) = x;
        return p[i] = 7;
}

int
division(int *p, int i, int a, int b)
{
        p[i] = a / b;
        p[i] = a << b;
        return p[i] = a % b;
}

int
calls(int *p, int i)
{
        p[i] = g();
        p[3] = g();
        p[5000] = g();
        (p + 1)[i] = g();
        return p[i] = g();
}

int
branches(int *p, int i, int a, int b, int c)
{
        p[i] = a && b;
        p[i] = a || b;
        p[i] = c ? a : b;
        return 0;
}

int
pointers(int **pp, int i, int j, int x)
{
        pp[i][j] = x;
        pp[i] = 0;
        *(pp + i) = pp[j];
        return 0;
}

int
pressure(int *p, int i, int a)
{
        p[i] = a + (a + (a + (a + (a + (a + (a + (a + (a))))))));
        p[i] =
            a +
            (a +
             (a +
              (a + (a + (a + (a + (a + (a + (a + (a + (a + (a + (a)))))))))))));
        return 0;
}
