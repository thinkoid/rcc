/* The integer promotions, 6.3.1.1p2, under every operator and on each
   side: a narrow operand is converted to int, or to the common type in
   one conversion, and its object is read by its extending load, movsbl
   -1(%rbp), %eax, and no operand in memory; c + l on a char and a long
   is the one movsbq.  A shift's count is promoted on its own.  ! and the
   operands of && and || keep their own types, tested at their width. */
int
arithmetic(void)
{
        char c = 1;
        int i = 2;
        return (c + i) + (i - c) + (c * 5) + (c / i) + (i % c) + (c & i) +
               (c | 3) + (c ^ i);
}

int
both_narrow(void)
{
        char c = 1;
        short s = 2;
        unsigned char u = 3;
        return (c + s) * (u - c) + (s << u) + (u >> c);
}

long
with_long(void)
{
        char c = 1;
        unsigned char u = 2;
        short s = 3;
        long l = 4;
        return (c + l) + (l * u) + (s - l) + (l / s);
}

unsigned
with_unsigned(void)
{
        signed char c = -1;
        unsigned short s = 2;
        unsigned u = 3;
        return (c + u) + (u / s) + (s % u);
}

int
comparisons(void)
{
        char c = 1;
        short s = 2;
        unsigned char u = 3;
        return (c < s) + (u == c) + (s >= 5) + (5 > u) + (c != 0);
}

int
shifts(void)
{
        char c = 1;
        short n = 2;
        long l = 3;
        return (c << n) + (l >> c) + (c >> 1) + (c << 33);
}

int
unary(void)
{
        char c = 1;
        unsigned short s = 2;
        return -c + ~s + +c + !c + !s;
}

int
logical(void)
{
        char c = 1;
        short s = 2;
        unsigned char u = 3;
        return (c && s) + (u || c) + (c && 5);
}

int
identities(void)
{
        char c = 1;
        unsigned char u = 2;
        return (c + 0) + (u * 1) + (c | 0);
}
