/* The width rule at 32 bits, the design's tables: a 32-bit write of an
   unsigned value clears the upper half, so it is valid to 64 bits and
   its widening writes nothing; a conversion caps the width at its own,
   so (unsigned) (a + b) and (unsigned) (a < b) are valid to 32 alone and
   their widening is movl %eax, %eax, mov w0, w0; an assignment's value
   is its right operand's, converted to the place's type: c = a on an
   unsigned c and an int a is the change of sign, valid to 32, and c = 5
   a constant of c's type, valid to 64 read unsigned. */
long
assigned_from_int(void)
{
        unsigned c;
        int a = 1;
        return c = a;
}

long
assigned_from_unsigned(void)
{
        unsigned c;
        unsigned a = 1;
        return c = a;
}

long
sum_of_unsigneds(void)
{
        long l = 1;
        unsigned a = 2;
        unsigned b = 3;
        return l + (a + b);
}

long
assigned_constant(void)
{
        unsigned c;
        return c = 5;
}

long
cast_of_sum(void)
{
        unsigned a = 1;
        unsigned b = 2;
        return (unsigned) (a + b);
}

long
cast_of_comparison(void)
{
        int a = 1;
        int b = 2;
        return (unsigned) (a < b);
}

long
signed_assigned_unsigned(void)
{
        int c;
        unsigned u = 1;
        return c = u;
}

long
cast_assigned(void)
{
        unsigned u;
        unsigned v = 1;
        return u = (unsigned) v;
}

long
change_of_sign_widened(void)
{
        unsigned u = 1;
        return (long) (int) u;
}

long
signed_change_of_sign_widened(void)
{
        int i = 1;
        return (unsigned long) (unsigned) i;
}

unsigned long
widened_then_added(void)
{
        unsigned a = 1;
        unsigned b = 2;
        return (unsigned long) (a + b) + (a - b);
}

long
operations(void)
{
        unsigned a = 7;
        unsigned b = 2;
        return (a * 5) + (a / b) + (a % b) + (a >> b) + ~a + -a;
}

long
remainder(void)
{
        unsigned a = 7;
        unsigned b = 2;
        return a % b;
}

unsigned long
quotient(void)
{
        unsigned a = 7;
        unsigned b = 2;
        return a / b;
}

int
compared_in_long(void)
{
        long l = 1;
        unsigned i = 2;
        return l < i + 1;
}

int
compared_unsigned_long(void)
{
        unsigned long l = 1;
        int i = 2;
        return l < i + 1;
}

int
stored(void)
{
        long l;
        unsigned a = 7;
        unsigned b = 2;
        l = a + b;
        return 0;
}

long
long_assigned_unsigned(void)
{
        long c;
        unsigned a = 7;
        return c = a;
}

long
unsigned_assigned_long(void)
{
        unsigned c;
        long a = 7;
        return c = a;
}
