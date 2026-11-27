/* A call's result of each type negated. */
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
negated_char(void)
{
        return -result_char();
}

signed char
negated_signed_char(void)
{
        return -result_signed_char();
}

unsigned char
negated_unsigned_char(void)
{
        return -result_unsigned_char();
}

short
negated_short(void)
{
        return -result_short();
}

unsigned short
negated_unsigned_short(void)
{
        return -result_unsigned_short();
}

int
negated_int(void)
{
        return -result_int();
}

unsigned
negated_unsigned(void)
{
        return -result_unsigned();
}

long
negated_long(void)
{
        return -result_long();
}

unsigned long
negated_unsigned_long(void)
{
        return -result_unsigned_long();
}

long long
negated_long_long(void)
{
        return -result_long_long();
}

unsigned long long
negated_unsigned_long_long(void)
{
        return -result_unsigned_long_long();
}
