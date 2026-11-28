/* Pointer arithmetic as a value, 6.5.7p9: the stride the pointee's size,
   a constant's bytes the immediate, addq $12, %rax and add x0, x0, #12,
   an index's the form, leaq (%rax,%rcx,4), %rax and add x0, x0, w1,
   sxtw #2, the index first kept first, and p - i multiplied, imulq $4,
   %rcx, %rcx. */
int g(void);

int *
constants(int *p)
{
        return p + 3;
}

int *
constant_first(int *p)
{
        return 3 + p;
}

int *
index(int *p, int i)
{
        return p + i;
}

int *
index_first(int *p, int i)
{
        return i + p;
}

int *
subtracted(int *p, int i)
{
        return p - i;
}

int *
subtracted_constant(int *p)
{
        return p - 3;
}

int *
long_index(int *p, long l)
{
        return p + l - l;
}

int *
unsigned_index(int *p, unsigned u)
{
        return p + u;
}

int *
call_index(int *p)
{
        return p + g();
}

int *
expression_index(int *p, int a, int b)
{
        return p + (a + b) + (a / b);
}

int *
chained(int *p, int i)
{
        return p + 1 + 2 + (p + 1 + i + 2 - p);
}

int *
wide(int *p)
{
        return p + 1000000000;
}

int *
wide_first(int *p)
{
        return 1000000000 + p;
}

int **
pointers(int **pp, int i)
{
        return pp + i + 1;
}

int *
addresses(int *p, int i)
{
        if (&p[3] == &p[0])
                return &p[i];
        return &*(p - 3);
}

int *
local_address(int i)
{
        int x = 1;
        if (&x + 1 == &x + i)
                return &x + 2;
        return &x - 1;
}
