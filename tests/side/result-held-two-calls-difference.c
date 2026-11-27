/* Two results of one type, the first less the second. */
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
two_calls_difference_char(void)
{
        return result_char() - result_char();
}

signed char
two_calls_difference_signed_char(void)
{
        return result_signed_char() - result_signed_char();
}

unsigned char
two_calls_difference_unsigned_char(void)
{
        return result_unsigned_char() - result_unsigned_char();
}

short
two_calls_difference_short(void)
{
        return result_short() - result_short();
}

unsigned short
two_calls_difference_unsigned_short(void)
{
        return result_unsigned_short() - result_unsigned_short();
}

int
two_calls_difference_int(void)
{
        return result_int() - result_int();
}

unsigned
two_calls_difference_unsigned(void)
{
        return result_unsigned() - result_unsigned();
}

long
two_calls_difference_long(void)
{
        return result_long() - result_long();
}

unsigned long
two_calls_difference_unsigned_long(void)
{
        return result_unsigned_long() - result_unsigned_long();
}

long long
two_calls_difference_long_long(void)
{
        return result_long_long() - result_long_long();
}

unsigned long long
two_calls_difference_unsigned_long_long(void)
{
        return result_unsigned_long_long() - result_unsigned_long_long();
}
