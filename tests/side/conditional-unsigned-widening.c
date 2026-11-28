/* A widening of an unsigned c ? y : n, which rcc never hands to the
   arms: one extension after the join, movl %eax, %eax, where the join is
   not valid to the new width, the lesser of the arms' as each left it.
   An arm loaded by a 32-bit write of an unsigned value is valid to 64
   bits, one converted to unsigned from an int is valid to 32, and an
   arm a mov brought into the join is valid as the mov wrote it: a call's
   result moved into a remainder's edx needs no extension. */
int g(int x);
static unsigned gu;
extern unsigned char gc;

long
unsigned_arms(void)
{
        int a;
        unsigned y;
        unsigned n;
        return a ? y : n;
}

long
int_and_unsigned(void)
{
        int a;
        int y;
        unsigned n;
        return a ? y : n;
}

long
unsigned_and_char(void)
{
        int a;
        unsigned y;
        char n;
        return a ? y : n;
}

long
unsigned_and_unsigned_char(void)
{
        int a;
        unsigned y;
        unsigned char n;
        return a ? y : n;
}

long
unsigned_short_and_unsigned(void)
{
        int a;
        unsigned short y;
        unsigned n;
        return a ? y : n;
}

long
unsigned_and_comparison(void)
{
        int a;
        unsigned y;
        unsigned n;
        return a ? y : n < y;
}

long
unsigned_and_sum(void)
{
        int a;
        unsigned y;
        unsigned n;
        return a ? y : n + y;
}

long
int_and_unsigned_constant(void)
{
        int a;
        int i;
        return a ? i : 5u;
}

long
unsigned_and_minus_one(void)
{
        int a;
        unsigned u;
        return a ? u : -1;
}

unsigned long
remainder_returned(void)
{
        int a;
        unsigned b;
        unsigned c;
        return a ? b % c : c;
}

int
remainder_and_call_stored(void)
{
        int a;
        unsigned y;
        unsigned n;
        long z;
        z = a ? y % n : g(a);
        return 0;
}

long
cast(void)
{
        int a;
        unsigned y;
        unsigned n;
        return (long) (a ? y : n);
}

long
constants(void)
{
        int a;
        return a ? -1 : 2u;
}

long
high_constant(void)
{
        int a;
        return a ? 0x80000000 : -1;
}

long
short_constant(void)
{
        int a;
        return a ? (short) -1 : 2u;
}

int
constants_stored(void)
{
        int a;
        long z;
        z = a ? -1 : 2u;
        return 0;
}

long
folded(void)
{
        return 1 ? -1 : 2u;
}

long
folded_second(void)
{
        return 0 ? 2u : -1;
}

long
nested(void)
{
        int a;
        int b;
        char c;
        long l;
        unsigned u;
        return a ? (b ? c : u) : l;
}

long
objects(void)
{
        int a;
        unsigned y;
        return a ? gu : y;
}

long
object_declared(void)
{
        int a;
        unsigned y;
        return a ? y : gc;
}

long
objects_stored(void)
{
        int a;
        long z;
        z = a ? gu : g(a);
        return z;
}
