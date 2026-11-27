/* A call's result of each type discarded: nothing after the call. */
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
discarded_char(void)
{
        result_char();
        return 0;
}

int
discarded_signed_char(void)
{
        result_signed_char();
        return 0;
}

int
discarded_unsigned_char(void)
{
        result_unsigned_char();
        return 0;
}

int
discarded_short(void)
{
        result_short();
        return 0;
}

int
discarded_unsigned_short(void)
{
        result_unsigned_short();
        return 0;
}

int
discarded_int(void)
{
        result_int();
        return 0;
}

int
discarded_unsigned(void)
{
        result_unsigned();
        return 0;
}

int
discarded_long(void)
{
        result_long();
        return 0;
}

int
discarded_unsigned_long(void)
{
        result_unsigned_long();
        return 0;
}

int
discarded_long_long(void)
{
        result_long_long();
        return 0;
}

int
discarded_unsigned_long_long(void)
{
        result_unsigned_long_long();
        return 0;
}
