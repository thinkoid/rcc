/* A call's result of each type as a condition, tested at its own
   type: cmpb $0, %al on x86-64, and on aarch64 tst w0, #0xff for a
   value valid to less than 32 bits, cbz otherwise. */
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
conditions_char(void)
{
        if (result_char())
                return 1;
        return 0;
}

int
conditions_signed_char(void)
{
        if (result_signed_char())
                return 1;
        return 0;
}

int
conditions_unsigned_char(void)
{
        if (result_unsigned_char())
                return 1;
        return 0;
}

int
conditions_short(void)
{
        if (result_short())
                return 1;
        return 0;
}

int
conditions_unsigned_short(void)
{
        if (result_unsigned_short())
                return 1;
        return 0;
}

int
conditions_int(void)
{
        if (result_int())
                return 1;
        return 0;
}

int
conditions_unsigned(void)
{
        if (result_unsigned())
                return 1;
        return 0;
}

int
conditions_long(void)
{
        if (result_long())
                return 1;
        return 0;
}

int
conditions_unsigned_long(void)
{
        if (result_unsigned_long())
                return 1;
        return 0;
}

int
conditions_long_long(void)
{
        if (result_long_long())
                return 1;
        return 0;
}

int
conditions_unsigned_long_long(void)
{
        if (result_unsigned_long_long())
                return 1;
        return 0;
}
