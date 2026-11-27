/* (long) (unsigned) g() of each result type: a narrow result
   extended to 32 bits by its own sign, then the widening of the
   unsigned, movl %eax, %eax and mov w0, w0, a zero extension's 64
   bits capped to 32 by the inner conversion. */
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
unsigned_widened_char(void)
{
        return (long) (unsigned) result_char();
}

long
unsigned_widened_signed_char(void)
{
        return (long) (unsigned) result_signed_char();
}

long
unsigned_widened_unsigned_char(void)
{
        return (long) (unsigned) result_unsigned_char();
}

long
unsigned_widened_short(void)
{
        return (long) (unsigned) result_short();
}

long
unsigned_widened_unsigned_short(void)
{
        return (long) (unsigned) result_unsigned_short();
}

long
unsigned_widened_int(void)
{
        return (long) (unsigned) result_int();
}

long
unsigned_widened_unsigned(void)
{
        return (long) (unsigned) result_unsigned();
}

long
unsigned_widened_long(void)
{
        return (long) (unsigned) result_long();
}

long
unsigned_widened_unsigned_long(void)
{
        return (long) (unsigned) result_unsigned_long();
}

long
unsigned_widened_long_long(void)
{
        return (long) (unsigned) result_long_long();
}

long
unsigned_widened_unsigned_long_long(void)
{
        return (long) (unsigned) result_unsigned_long_long();
}
