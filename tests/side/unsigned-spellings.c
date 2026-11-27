/* The spellings of unsigned and unsigned long, 6.7.3.1p2, each as a
   local's type, a function's result type and a cast's type name; and
   each read as a long, which a 32-bit unsigned is by its movl, valid to
   64 bits, and shifted right, shrl or shrq, lsr. */

unsigned
u0(void)
{
        unsigned x = 0;
        return (unsigned) x;
}

long
u0_long(void)
{
        unsigned x = 0;
        return x >> 1;
}

unsigned int
u1(void)
{
        unsigned int x = 1;
        return (unsigned int) x;
}

long
u1_long(void)
{
        unsigned int x = 1;
        return x >> 1;
}

int unsigned
u2(void)
{
        int unsigned x = 2;
        return (int unsigned) x;
}

long
u2_long(void)
{
        int unsigned x = 2;
        return x >> 1;
}

unsigned long
ul3(void)
{
        unsigned long x = 3;
        return (unsigned long) x;
}

long
ul3_long(void)
{
        unsigned long x = 3;
        return x >> 1;
}

long unsigned
ul4(void)
{
        long unsigned x = 4;
        return (long unsigned) x;
}

long
ul4_long(void)
{
        long unsigned x = 4;
        return x >> 1;
}

unsigned long int
ul5(void)
{
        unsigned long int x = 5;
        return (unsigned long int) x;
}

long
ul5_long(void)
{
        unsigned long int x = 5;
        return x >> 1;
}

long unsigned int
ul6(void)
{
        long unsigned int x = 6;
        return (long unsigned int) x;
}

long
ul6_long(void)
{
        long unsigned int x = 6;
        return x >> 1;
}

int long unsigned
ul7(void)
{
        int long unsigned x = 7;
        return (int long unsigned) x;
}

long
ul7_long(void)
{
        int long unsigned x = 7;
        return x >> 1;
}

unsigned long long
ul8(void)
{
        unsigned long long x = 8;
        return (unsigned long long) x;
}

long
ul8_long(void)
{
        unsigned long long x = 8;
        return x >> 1;
}

long long unsigned
ul9(void)
{
        long long unsigned x = 9;
        return (long long unsigned) x;
}

long
ul9_long(void)
{
        long long unsigned x = 9;
        return x >> 1;
}

long unsigned long
ul10(void)
{
        long unsigned long x = 10;
        return (long unsigned long) x;
}

long
ul10_long(void)
{
        long unsigned long x = 10;
        return x >> 1;
}

unsigned long long int
ul11(void)
{
        unsigned long long int x = 11;
        return (unsigned long long int) x;
}

long
ul11_long(void)
{
        unsigned long long int x = 11;
        return x >> 1;
}

int unsigned long long
ul12(void)
{
        int unsigned long long x = 12;
        return (int unsigned long long) x;
}

long
ul12_long(void)
{
        int unsigned long long x = 12;
        return x >> 1;
}

long long int unsigned
ul13(void)
{
        long long int unsigned x = 13;
        return (long long int unsigned) x;
}

long
ul13_long(void)
{
        long long int unsigned x = 13;
        return x >> 1;
}
