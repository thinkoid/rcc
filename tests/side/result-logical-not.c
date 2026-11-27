/* !g() of each result type as a value: its test at its own type,
   then the value of the flags. */
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
logical_not_char(void)
{
        return !result_char();
}

int
logical_not_signed_char(void)
{
        return !result_signed_char();
}

int
logical_not_unsigned_char(void)
{
        return !result_unsigned_char();
}

int
logical_not_short(void)
{
        return !result_short();
}

int
logical_not_unsigned_short(void)
{
        return !result_unsigned_short();
}

int
logical_not_int(void)
{
        return !result_int();
}

int
logical_not_unsigned(void)
{
        return !result_unsigned();
}

int
logical_not_long(void)
{
        return !result_long();
}

int
logical_not_unsigned_long(void)
{
        return !result_unsigned_long();
}

int
logical_not_long_long(void)
{
        return !result_long_long();
}

int
logical_not_unsigned_long_long(void)
{
        return !result_unsigned_long_long();
}
