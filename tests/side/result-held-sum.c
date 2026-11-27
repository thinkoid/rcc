/* A local and a result of one type summed: the result extended in
   rax and x0 before the operator's row at the common type. */
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
sum_char(void)
{
        char x;
        return x + result_char();
}

signed char
sum_signed_char(void)
{
        signed char x;
        return x + result_signed_char();
}

unsigned char
sum_unsigned_char(void)
{
        unsigned char x;
        return x + result_unsigned_char();
}

short
sum_short(void)
{
        short x;
        return x + result_short();
}

unsigned short
sum_unsigned_short(void)
{
        unsigned short x;
        return x + result_unsigned_short();
}

int
sum_int(void)
{
        int x;
        return x + result_int();
}

unsigned
sum_unsigned(void)
{
        unsigned x;
        return x + result_unsigned();
}

long
sum_long(void)
{
        long x;
        return x + result_long();
}

unsigned long
sum_unsigned_long(void)
{
        unsigned long x;
        return x + result_unsigned_long();
}

long long
sum_long_long(void)
{
        long long x;
        return x + result_long_long();
}

unsigned long long
sum_unsigned_long_long(void)
{
        unsigned long long x;
        return x + result_unsigned_long_long();
}
