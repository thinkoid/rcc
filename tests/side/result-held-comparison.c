/* A local compared with a result of one type. */
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

int
comparison_char(void)
{
        char x;
        return x < result_char();
}

int
comparison_signed_char(void)
{
        signed char x;
        return x < result_signed_char();
}

int
comparison_unsigned_char(void)
{
        unsigned char x;
        return x < result_unsigned_char();
}

int
comparison_short(void)
{
        short x;
        return x < result_short();
}

int
comparison_unsigned_short(void)
{
        unsigned short x;
        return x < result_unsigned_short();
}

int
comparison_int(void)
{
        int x;
        return x < result_int();
}

int
comparison_unsigned(void)
{
        unsigned x;
        return x < result_unsigned();
}

int
comparison_long(void)
{
        long x;
        return x < result_long();
}

int
comparison_unsigned_long(void)
{
        unsigned long x;
        return x < result_unsigned_long();
}

int
comparison_long_long(void)
{
        long long x;
        return x < result_long_long();
}

int
comparison_unsigned_long_long(void)
{
        unsigned long long x;
        return x < result_unsigned_long_long();
}
