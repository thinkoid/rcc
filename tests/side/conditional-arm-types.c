/* c ? y : n on arms of two types, 6.5.16p6: each arm converted to the
   type of the usual arithmetic conversions of the two, in one conversion
   on its own path, so that the extending load of a narrow or an int arm
   stands beside an unsigned arm's movl, and nothing follows the join;
   two narrow arms make an int, and a constant arm is the constant of
   the common type.  The join's mov is at the conditional's width. */
static char gy;
extern long gn;
extern unsigned gu;

int
narrow_arms(void)
{
        int a;
        char y;
        char n;
        return a ? y : n;
}

long
narrow_and_long(void)
{
        int a;
        char y;
        long n;
        return a ? y : n;
}

long
long_and_narrow(void)
{
        int a;
        long y;
        char n;
        return a ? y : n;
}

long
int_and_long(void)
{
        int a;
        int y;
        long n;
        return a ? y : n;
}

long
long_and_unsigned(void)
{
        int a;
        long y;
        unsigned n;
        return a ? y : n;
}

long
unsigned_char_and_long(void)
{
        int a;
        unsigned char y;
        long n;
        return a ? y : n;
}

long
unsigned_and_long(void)
{
        int a;
        unsigned y;
        long n;
        return a ? y : n;
}

unsigned
char_and_unsigned(void)
{
        int a;
        char y;
        unsigned n;
        return a ? y : n;
}

long
remainder_of_long(void)
{
        int a;
        long b;
        long c;
        return a ? b % c : c;
}

long
long_count(void)
{
        int a;
        long x;
        long c;
        int d;
        return x << (a ? c : d);
}

int
narrow_count(void)
{
        int a;
        int x;
        char c;
        char d;
        return x << (a ? c : d);
}

long
long_remainder_operand(void)
{
        int a;
        long b;
        long c;
        long l;
        return l + (a ? b % c : c);
}

int
narrowed(void)
{
        int a;
        char z;
        long y;
        long n;
        z = a ? y : n;
        return 0;
}

int g(int x);

int
argument(void)
{
        int a;
        char c;
        long l;
        return g(a ? c : l);
}

long
constants_of_long(void)
{
        int a;
        return a ? 1 : 2L;
}

long
constants_of_unsigned_long(void)
{
        int a;
        return a ? -1 : 2UL;
}

long
unsigned_and_long_constants(void)
{
        int a;
        return a ? 1u : -1L;
}

int
long_returned_as_int(void)
{
        int a;
        return a ? 0x123456789 : 1;
}

int
long_as_condition(void)
{
        int a;
        if (a ? 0 : 0L)
                return 1;
        return 0;
}

long
char_and_long_constant(void)
{
        int a;
        char c;
        return a ? c : 5L;
}

int
folded(void)
{
        return 1 ? -1 : 2u;
}

long
objects(void)
{
        int a;
        return a ? gy : gn;
}

long
object_declared_and_unsigned(void)
{
        int a;
        return a ? gn : gu;
}
