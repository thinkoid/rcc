/* The unsigned operators at both views, C6 of the survey: a division
   clears rdx, xorl %edx, %edx, at either width, then divl or divq, and
   aarch64's udiv, with msub for %; >> is shrl, shrq and lsr; a
   comparison's condition is b be a ae on x86-64, lo ls hi hs on aarch64.
   A remainder of 32 bits returned as a long is moved from rdx at 64
   bits, valid to 64 already. */
unsigned
quotients(void)
{
        unsigned a = 7;
        unsigned b = 2;
        return (a / b) + (a / 5) + (5 / a) + ((a + b) / (a - b));
}

unsigned
remainders(void)
{
        unsigned a = 7;
        unsigned b = 2;
        return (a % b) + (a % 5) + (5 % a) + ((a + b) % (a - b));
}

unsigned long
long_quotients(void)
{
        unsigned long a = 7;
        unsigned long b = 2;
        return (a / b) + (a % b) + (a / 0x123456789) + ((a + b) % (a - b));
}

unsigned
right_shifts(void)
{
        unsigned a = 7;
        unsigned b = 2;
        return (a >> b) + (a >> 5) + (5u >> a) + ((a + b) >> (a - b));
}

unsigned long
long_right_shifts(void)
{
        unsigned long a = 7;
        int b = 2;
        return (a >> b) + (a >> 40) + (a >> (b + 1));
}

int
comparisons(void)
{
        unsigned a = 7;
        unsigned b = 2;
        return (a < b) + (a > 5) + (5 <= a) + ((a + b) >= (a - b)) + (a == b) +
               (a != 5);
}

int
long_comparisons(void)
{
        unsigned long a = 7;
        unsigned long b = 2;
        return (a < b) + (a > 5) + (5 <= a) + ((a + b) >= (a - b));
}

int
mixed_signs(void)
{
        int i = -1;
        unsigned u = 0;
        return (i < u) + (u > i) + (-1 < 0u) + (-1L < 0u) + (-1 < 0ul);
}

int
conditions(void)
{
        unsigned a = 7;
        unsigned b = 2;
        if (a < b)
                return 1;
        if (a >= 5)
                return 2;
        while (a > b)
                a = a - 1;
        return 0;
}

long
held(void)
{
        unsigned a = 7;
        unsigned b = 2;
        unsigned c = 3;
        return a + b / c + (a >> c) * (b % (a - c));
}

long
remainder_returned(void)
{
        unsigned a = 7;
        unsigned b = 2;
        return a % b;
}
