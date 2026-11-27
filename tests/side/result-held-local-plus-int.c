/* A local of each type plus an int result: the narrow local's
   extending load spilled whole, 8 bytes, and read in its slot. */
int int_result(void);

int
local_plus_int_char(void)
{
        char x;
        return x + int_result();
}

int
local_plus_int_signed_char(void)
{
        signed char x;
        return x + int_result();
}

int
local_plus_int_unsigned_char(void)
{
        unsigned char x;
        return x + int_result();
}

int
local_plus_int_short(void)
{
        short x;
        return x + int_result();
}

int
local_plus_int_unsigned_short(void)
{
        unsigned short x;
        return x + int_result();
}

int
local_plus_int_int(void)
{
        int x;
        return x + int_result();
}

int
local_plus_int_unsigned(void)
{
        unsigned x;
        return x + int_result();
}

int
local_plus_int_long(void)
{
        long x;
        return x + int_result();
}

int
local_plus_int_unsigned_long(void)
{
        unsigned long x;
        return x + int_result();
}

int
local_plus_int_long_long(void)
{
        long long x;
        return x + int_result();
}

int
local_plus_int_unsigned_long_long(void)
{
        unsigned long long x;
        return x + int_result();
}
