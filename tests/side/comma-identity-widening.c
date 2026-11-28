/* A widening over a comma whose right operand is an identity's other
   operand, 0u + b on an int b, which is b converted to unsigned: the
   comma's value is that conversion's, valid to 32 bits, rcc's cap in
   converted(), so the widening writes movl %ecx, %ecx and mov w1, w1.
   The identity alone on the right is the conversion of the object,
   which the widening reads in one load and extends no further. */
static int g;

int
comma(void)
{
        int b;
        unsigned long p;
        return p != (b, 0u + b);
}

int
comma_held(void)
{
        int b;
        unsigned long p;
        return p != ((b % 200), (0u + b));
}

int
alone(void)
{
        int b;
        unsigned long p;
        return p != (0u + b);
}

int
left_identity(void)
{
        int b;
        unsigned long p;
        return p != (b, b + 0u);
}

unsigned long
object(void)
{
        int b;
        return (b, 0u + g);
}

long
signed_identity(void)
{
        unsigned u;
        int b;
        return (b, 0 + (int) u);
}
