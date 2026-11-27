/* g() == 0 of each result type as a condition, which compares
   promoted operands, so a narrow result is extended first. */
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
compared_with_zero_char(void)
{
        if (result_char() == 0)
                return 1;
        return 0;
}

int
compared_with_zero_signed_char(void)
{
        if (result_signed_char() == 0)
                return 1;
        return 0;
}

int
compared_with_zero_unsigned_char(void)
{
        if (result_unsigned_char() == 0)
                return 1;
        return 0;
}

int
compared_with_zero_short(void)
{
        if (result_short() == 0)
                return 1;
        return 0;
}

int
compared_with_zero_unsigned_short(void)
{
        if (result_unsigned_short() == 0)
                return 1;
        return 0;
}

int
compared_with_zero_int(void)
{
        if (result_int() == 0)
                return 1;
        return 0;
}

int
compared_with_zero_unsigned(void)
{
        if (result_unsigned() == 0)
                return 1;
        return 0;
}

int
compared_with_zero_long(void)
{
        if (result_long() == 0)
                return 1;
        return 0;
}

int
compared_with_zero_unsigned_long(void)
{
        if (result_unsigned_long() == 0)
                return 1;
        return 0;
}

int
compared_with_zero_long_long(void)
{
        if (result_long_long() == 0)
                return 1;
        return 0;
}

int
compared_with_zero_unsigned_long_long(void)
{
        if (result_unsigned_long_long() == 0)
                return 1;
        return 0;
}
