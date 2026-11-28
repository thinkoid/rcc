/* The subscript read, *(p + n): the form of the access, 12(%rax) and
   [x0, #12], (%rax,%rcx,4) and [x0, w1, sxtw #2], the index first the
   same operand, a constant held where the access does not reach it,
   mov x1, #5000; ldr w0, [x0, x1, lsl #2]. */
int g(void);
unsigned gu(void);

int
constant(int *p)
{
        return p[3] + p[0] + 3 [p];
}

int
index(int *p, int i)
{
        return p[i] + i[p];
}

int
negative(int *p)
{
        return *(p - 3) + p[-3];
}

int
long_index(int *p, long l, unsigned u)
{
        return p[l] + p[u];
}

int
call_index(int *p)
{
        return p[g()] + p[gu()];
}

int
pinned_index(int *p, int a, int b)
{
        return p[a / b] + p[a << b];
}

int
chained(int *p, int i)
{
        return *(p + 1 + i + 2) + *(p + 1 + 2) + (p + 1)[2];
}

int
far(int *p)
{
        return p[5000] + (p + 1)[5000] + p[1023] + p[1024];
}

int
pointers(int **pp, int i, int j)
{
        return pp[i][j] + *pp[i] + **(pp + 1);
}

int
local(int i)
{
        int x = 1;
        return (&x)[i] + (&x)[0] + *(&x + 0);
}

int
cancelled(int *p, int i)
{
        return *&*(p + i);
}

int
narrow_index(
    int *p, char c, unsigned char uc, short s, unsigned short us, int i)
{
        return p[c] + p[uc] + p[s] + p[us] + p[(char) i] + p[(short) i] +
               p[(long) c];
}

int
rooted(int *p, int i)
{
        return (&p[1])[2] + (&p[1])[i] + (&*p)[3] + (&p[1])[5000] +
               *(&p[2] - 1);
}

int *
rooted_values(int *p, int i)
{
        if (&(&p[1])[2] == &(&p[1])[i])
                return &(&p[1])[i];
        return &p[1] + 2;
}
