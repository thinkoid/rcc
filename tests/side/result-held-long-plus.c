/* A long local held across a call of each result type. */
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

long
long_plus_char(void)
{
        long l;
        return l + result_char();
}

long
long_plus_signed_char(void)
{
        long l;
        return l + result_signed_char();
}

long
long_plus_unsigned_char(void)
{
        long l;
        return l + result_unsigned_char();
}

long
long_plus_short(void)
{
        long l;
        return l + result_short();
}

long
long_plus_unsigned_short(void)
{
        long l;
        return l + result_unsigned_short();
}

long
long_plus_int(void)
{
        long l;
        return l + result_int();
}

long
long_plus_unsigned(void)
{
        long l;
        return l + result_unsigned();
}

long
long_plus_long(void)
{
        long l;
        return l + result_long();
}

long
long_plus_unsigned_long(void)
{
        long l;
        return l + result_unsigned_long();
}

long
long_plus_long_long(void)
{
        long l;
        return l + result_long_long();
}

long
long_plus_unsigned_long_long(void)
{
        long l;
        return l + result_unsigned_long_long();
}
