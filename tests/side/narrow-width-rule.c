/* The width rule with the narrow types, D3.2: a conversion to a narrow
   type writes nothing and leaves the value valid to its width alone, so
   its widening extends in the register, movsbl %al, %eax, sxtb w0, w0;
   an object read by its extending load is valid to 32 bits, but a
   signed narrow object read so into an unsigned of 32 bits is valid to
   64 as that unsigned, so u = c writes no movl %eax, %eax where u = (int)
   c and u = (signed char) (a + b) do, and an unsigned char read into an
   unsigned writes one. */
long
signed_char_to_unsigned(void)
{
        unsigned u;
        signed char c = -1;
        return u = c;
}

long
short_to_unsigned(void)
{
        unsigned u;
        short s = -1;
        return u = s;
}

long
int_of_signed_char_to_unsigned(void)
{
        unsigned u;
        signed char c = -1;
        return u = (int) c;
}

long
narrowed_sum_to_unsigned(void)
{
        int a = 1;
        int b = 2;
        unsigned u;
        return u = (signed char) (a + b);
}

long
unsigned_char_to_unsigned(void)
{
        unsigned u;
        unsigned char c = 255;
        return u = c;
}

int
narrowed_sum(void)
{
        int a = 1;
        int b = 2;
        return (char) (a + b) + (signed char) (a + b) +
               (unsigned char) (a + b) + (short) (a + b) +
               (unsigned short) (a + b);
}

long
narrowed_long_sum(void)
{
        long a = 1;
        long b = 2;
        return (char) (a + b) + (signed char) (a + b) +
               (unsigned char) (a + b) + (short) (a + b) +
               (unsigned short) (a + b);
}

int
assigned_char(void)
{
        char c;
        int a = 1;
        return c = a;
}

long
assigned_char_long(void)
{
        unsigned char c;
        int a = 1;
        return c = a;
}

long
assigned_short_from_long(void)
{
        unsigned short c;
        long a = 1;
        return c = a;
}

long
assigned_signed_char_from_unsigned(void)
{
        signed char c;
        unsigned a = 1;
        return c = a;
}

int
narrowed_load(void)
{
        long a = 1;
        return (long) (char) a + (long) (unsigned char) a;
}

int
cast_own_type_twice(void)
{
        char a = 1;
        return (char) (int) a;
}

int
stored_sum(void)
{
        char c;
        int a = 1;
        c = a + 1;
        return c;
}
