/* Computed pointers compared and tested, at 64 bits, unsigned, and the
   difference of two pointers, a long, the subq and the division by the
   stride, movq $4, %rcx; cqto; idivq %rcx. */
int
compared(int *p, int *q, int i)
{
        return (p + i == q) + (p + i < q) + !(p + i) + (q - 1 != p);
}

int
tested(int *p, int i)
{
        if (p + i)
                return 1;
        return 0;
}

long
difference(int *p, int *q)
{
        return p - q;
}

long
difference_operand(int *p, int *q, long x)
{
        return x + (p - q);
}

int
difference_narrowed(int **pp, int **qq)
{
        return (int) (pp - qq) + (int) (pp + 1 - qq);
}
