/* A call's result of each type as an operand of + at int, its
   promotion or the usual conversion first, then the row. */
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
plus_int_char(void)
{
        int i;
        return result_char() + i;
}

int
plus_int_signed_char(void)
{
        int i;
        return result_signed_char() + i;
}

int
plus_int_unsigned_char(void)
{
        int i;
        return result_unsigned_char() + i;
}

int
plus_int_short(void)
{
        int i;
        return result_short() + i;
}

int
plus_int_unsigned_short(void)
{
        int i;
        return result_unsigned_short() + i;
}

int
plus_int_int(void)
{
        int i;
        return result_int() + i;
}

int
plus_int_unsigned(void)
{
        int i;
        return result_unsigned() + i;
}

int
plus_int_long(void)
{
        int i;
        return result_long() + i;
}

int
plus_int_unsigned_long(void)
{
        int i;
        return result_unsigned_long() + i;
}

int
plus_int_long_long(void)
{
        int i;
        return result_long_long() + i;
}

int
plus_int_unsigned_long_long(void)
{
        int i;
        return result_unsigned_long_long() + i;
}
