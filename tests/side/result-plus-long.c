/* A call's result of each type as an operand of + at long: a narrow
   or an int result extended to 64 bits, an unsigned one by movl
   %eax, %eax and mov w0, w0. */
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
plus_long_char(void)
{
        long l;
        return result_char() + l;
}

long
plus_long_signed_char(void)
{
        long l;
        return result_signed_char() + l;
}

long
plus_long_unsigned_char(void)
{
        long l;
        return result_unsigned_char() + l;
}

long
plus_long_short(void)
{
        long l;
        return result_short() + l;
}

long
plus_long_unsigned_short(void)
{
        long l;
        return result_unsigned_short() + l;
}

long
plus_long_int(void)
{
        long l;
        return result_int() + l;
}

long
plus_long_unsigned(void)
{
        long l;
        return result_unsigned() + l;
}

long
plus_long_long(void)
{
        long l;
        return result_long() + l;
}

long
plus_long_unsigned_long(void)
{
        long l;
        return result_unsigned_long() + l;
}

long
plus_long_long_long(void)
{
        long l;
        return result_long_long() + l;
}

long
plus_long_unsigned_long_long(void)
{
        long l;
        return result_unsigned_long_long() + l;
}
