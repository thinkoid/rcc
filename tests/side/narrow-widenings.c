/* The widening's builder over a narrow object, D3.4's first table: the
   object is read once, at the type rcc reads it at.  (long) (int) c is
   the one movsbq of a signed char c, ldrsb x0, and the one movzbl of an
   unsigned char, ldrb w0; (long) (unsigned) c of a signed char is
   movsbl alone, valid to 64 bits as an unsigned; (int) (unsigned char)
   c reads c as an unsigned char, movzbl.  (long) (int) u on an unsigned
   is movslq. */
long
long_of_int_of_char(void)
{
        char c = 1;
        return (long) (int) c;
}

long
long_of_int_of_signed_char(void)
{
        signed char c = 1;
        return (long) (int) c;
}

long
long_of_int_of_unsigned_char(void)
{
        unsigned char c = 1;
        return (long) (int) c;
}

long
long_of_unsigned_of_char(void)
{
        char c = 1;
        return (long) (unsigned) c;
}

long
long_of_unsigned_of_signed_char(void)
{
        signed char c = 1;
        return (long) (unsigned) c;
}

long
long_of_int_of_short(void)
{
        short s = 1;
        return (long) (int) s;
}

unsigned long
unsigned_long_of_int_of_char(void)
{
        char c = 1;
        return (unsigned long) (int) c;
}

int
unsigned_char_of_signed_char(void)
{
        signed char c = 1;
        return (unsigned char) c;
}

int
short_of_unsigned_char(void)
{
        unsigned char c = 1;
        return (short) c;
}

long
long_of_narrow(void)
{
        char c = 1;
        signed char d = 2;
        unsigned char e = 3;
        short s = 4;
        unsigned short t = 5;
        return (long) c + d + e + s + t;
}

long
long_of_unsigned_object(void)
{
        unsigned u = 1;
        return (long) (int) u;
}
