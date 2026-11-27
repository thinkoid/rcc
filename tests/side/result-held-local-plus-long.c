/* A local of each type plus a long result: the local widened to 64
   bits by its load before the spill, read in its slot at 64. */
long long_result(void);

long
local_plus_long_char(void)
{
        char x;
        return x + long_result();
}

long
local_plus_long_signed_char(void)
{
        signed char x;
        return x + long_result();
}

long
local_plus_long_unsigned_char(void)
{
        unsigned char x;
        return x + long_result();
}

long
local_plus_long_short(void)
{
        short x;
        return x + long_result();
}

long
local_plus_long_unsigned_short(void)
{
        unsigned short x;
        return x + long_result();
}

long
local_plus_long_int(void)
{
        int x;
        return x + long_result();
}

long
local_plus_long_unsigned(void)
{
        unsigned x;
        return x + long_result();
}

long
local_plus_long_long(void)
{
        long x;
        return x + long_result();
}

long
local_plus_long_unsigned_long(void)
{
        unsigned long x;
        return x + long_result();
}

long
local_plus_long_long_long(void)
{
        long long x;
        return x + long_result();
}

long
local_plus_long_unsigned_long_long(void)
{
        unsigned long long x;
        return x + long_result();
}
