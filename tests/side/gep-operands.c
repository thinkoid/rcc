/* A subscript as an operand and as a condition: x86-64 names the
   element in memory, addl (%rax,%rcx,4), %edx, cmpl $5, 12(%rax). */
int
operands(int *p, int i, int x)
{
        return (x + p[i]) + (p[i] - x) + (p[i] * 5) + (x / p[3]) + (p[i] < 5) +
               (5 < p[3]) + !p[i];
}

int
conditions(int *p, int i, int *q)
{
        if (p[i])
                return 1;
        if (p[3] & 1)
                return 2;
        while (p[i] && q[i])
                i = i + 1;
        return p[i] == 0;
}

/* An index that divides leaves the consumer's register to itself: x is
   loaded outside rax, which the division and the result take. */
int
pinned_index(int *p, int x, int a, int b)
{
        return (x ^ p[a / b]) + (x / p[a % b]);
}

int g();

long
pinned_count(long y, int *p, int x, int a, int b)
{
        return y << (x ^ p[g() && a / b]);
}
