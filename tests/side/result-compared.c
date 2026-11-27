/* A call's result of each type compared with 5, at the promoted
   type. */
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
compared_char(void)
{
        return result_char() < 5;
}

int
compared_signed_char(void)
{
        return result_signed_char() < 5;
}

int
compared_unsigned_char(void)
{
        return result_unsigned_char() < 5;
}

int
compared_short(void)
{
        return result_short() < 5;
}

int
compared_unsigned_short(void)
{
        return result_unsigned_short() < 5;
}

int
compared_int(void)
{
        return result_int() < 5;
}

int
compared_unsigned(void)
{
        return result_unsigned() < 5;
}

int
compared_long(void)
{
        return result_long() < 5;
}

int
compared_unsigned_long(void)
{
        return result_unsigned_long() < 5;
}

int
compared_long_long(void)
{
        return result_long_long() < 5;
}

int
compared_unsigned_long_long(void)
{
        return result_unsigned_long_long() < 5;
}
