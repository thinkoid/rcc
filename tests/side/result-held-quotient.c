/* A local divided by a result of one type. */
char result_char(void);
signed char result_signed_char(void);
unsigned char result_unsigned_char(void);
short result_short(void);
unsigned short result_unsigned_short(void);
int result_int(void);
unsigned result_unsigned(void);
long result_long(void);
unsigned long result_unsigned_long(void);
long long result_long_long(void);
unsigned long long result_unsigned_long_long(void);

char
quotient_char(void)
{
        char x;
        return x / result_char();
}

signed char
quotient_signed_char(void)
{
        signed char x;
        return x / result_signed_char();
}

unsigned char
quotient_unsigned_char(void)
{
        unsigned char x;
        return x / result_unsigned_char();
}

short
quotient_short(void)
{
        short x;
        return x / result_short();
}

unsigned short
quotient_unsigned_short(void)
{
        unsigned short x;
        return x / result_unsigned_short();
}

int
quotient_int(void)
{
        int x;
        return x / result_int();
}

unsigned
quotient_unsigned(void)
{
        unsigned x;
        return x / result_unsigned();
}

long
quotient_long(void)
{
        long x;
        return x / result_long();
}

unsigned long
quotient_unsigned_long(void)
{
        unsigned long x;
        return x / result_unsigned_long();
}

long long
quotient_long_long(void)
{
        long long x;
        return x / result_long_long();
}

unsigned long long
quotient_unsigned_long_long(void)
{
        unsigned long long x;
        return x / result_unsigned_long_long();
}
