/* A call's result of each type plus 1, returned at its own type. */
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
plus_one_char(void)
{
        return result_char() + 1;
}

signed char
plus_one_signed_char(void)
{
        return result_signed_char() + 1;
}

unsigned char
plus_one_unsigned_char(void)
{
        return result_unsigned_char() + 1;
}

short
plus_one_short(void)
{
        return result_short() + 1;
}

unsigned short
plus_one_unsigned_short(void)
{
        return result_unsigned_short() + 1;
}

int
plus_one_int(void)
{
        return result_int() + 1;
}

unsigned
plus_one_unsigned(void)
{
        return result_unsigned() + 1;
}

long
plus_one_long(void)
{
        return result_long() + 1;
}

unsigned long
plus_one_unsigned_long(void)
{
        return result_unsigned_long() + 1;
}

long long
plus_one_long_long(void)
{
        return result_long_long() + 1;
}

unsigned long long
plus_one_unsigned_long_long(void)
{
        return result_unsigned_long_long() + 1;
}
