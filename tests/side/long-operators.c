/* Every operator at 64 bits, C6 of the survey, with each class of
   operand on each side: a constant, a load and a computed value.  The
   mnemonics are the q forms on x86-64, cqto and idivq for a division,
   cmpq for a comparison, and the x names on aarch64; a long on the
   right is the instruction's operand in memory, addq -16(%rbp), %rax,
   an int widened to long is its movslq load and no operand in memory,
   and an immediate is the instruction's.  The left operand of && and
   || keeps its own type, tested at its own width. */
long
sums(void)
{
        long a = 1;
        long b = 2;
        return (a + b) + (a + 5) + (5 + a) + ((a - b) + (a * b));
}

long
differences(void)
{
        long a = 1;
        long b = 2;
        return (a - b) - (a - 5) - (5 - a) - ((a + b) - (a * b));
}

long
products(void)
{
        long a = 1;
        long b = 2;
        return (a * b) * (a * 5) * (5 * a) * ((a + b) * (a - b));
}

long
quotients(void)
{
        long a = 7;
        long b = 2;
        return (a / b) + (a / 5) + (5 / a) + ((a + b) / (a - b));
}

long
remainders(void)
{
        long a = 7;
        long b = 2;
        return (a % b) + (a % 5) + (5 % a) + ((a + b) % (a - b));
}

long
left_shifts(void)
{
        long a = 7;
        long b = 2;
        return (a << b) + (a << 5) + (5L << a) + ((a + b) << (a - b));
}

long
right_shifts(void)
{
        long a = 7;
        long b = 2;
        return (a >> b) + (a >> 5) + (5L >> a) + ((a + b) >> (a - b));
}

long
bitwise(void)
{
        long a = 7;
        long b = 2;
        return ((a & b) | (a & 5)) ^ ((5 | a) & ((a + b) ^ (a - b)));
}

long
negations(void)
{
        long a = 7;
        return -a + ~a + -(a + 1) + ~(a * 3);
}

int
comparisons(void)
{
        long a = 7;
        long b = 2;
        return (a < b) + (a > 5) + (5 <= a) + ((a + b) >= (a - b)) + (a == b) +
               (a != 5);
}

int
logical(void)
{
        long a = 7;
        int b = 2;
        return (a && b) + (b || a) + (a && 5L) + ((a + 1) || (a - 1)) + !a;
}

long
mixed(void)
{
        long a = 7;
        int i = 2;
        return (a + i) + (i + a) + (a / i) + (i % a) + (a < i);
}

long
held(void)
{
        long a = 7;
        long b = 2;
        long c = 3;
        return a + b / c + (a << c) * (b % (a - c));
}
