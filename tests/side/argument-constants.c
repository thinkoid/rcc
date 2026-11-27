/* A constant argument of each parameter type: the constant of the
   parameter's type, folded by the parser, so 300 to a char is 44,
   stored to the argument's slot as a constant of that type is to a
   local, movb $44, -1(%rbp), movabsq for a long no 32-bit immediate
   holds, and strb wzr for 0. */
int takes_char(char x);
int takes_signed_char(signed char x);
int takes_unsigned_char(unsigned char x);
int takes_short(short x);
int takes_unsigned_short(unsigned short x);
int takes_int(int x);
int takes_long(long x);

int
char_0(void)
{
        return takes_char(0);
}

int
char_1(void)
{
        return takes_char(1);
}

int
char_minus_1(void)
{
        return takes_char(-1);
}

int
char_300(void)
{
        return takes_char(300);
}

int
char_100000(void)
{
        return takes_char(100000);
}

int
char_hex_123456789(void)
{
        return takes_char(0x123456789);
}

int
signed_char_0(void)
{
        return takes_signed_char(0);
}

int
signed_char_1(void)
{
        return takes_signed_char(1);
}

int
signed_char_minus_1(void)
{
        return takes_signed_char(-1);
}

int
signed_char_300(void)
{
        return takes_signed_char(300);
}

int
signed_char_100000(void)
{
        return takes_signed_char(100000);
}

int
signed_char_hex_123456789(void)
{
        return takes_signed_char(0x123456789);
}

int
unsigned_char_0(void)
{
        return takes_unsigned_char(0);
}

int
unsigned_char_1(void)
{
        return takes_unsigned_char(1);
}

int
unsigned_char_minus_1(void)
{
        return takes_unsigned_char(-1);
}

int
unsigned_char_300(void)
{
        return takes_unsigned_char(300);
}

int
unsigned_char_100000(void)
{
        return takes_unsigned_char(100000);
}

int
unsigned_char_hex_123456789(void)
{
        return takes_unsigned_char(0x123456789);
}

int
short_0(void)
{
        return takes_short(0);
}

int
short_1(void)
{
        return takes_short(1);
}

int
short_minus_1(void)
{
        return takes_short(-1);
}

int
short_255(void)
{
        return takes_short(255);
}

int
short_300(void)
{
        return takes_short(300);
}

int
short_100000(void)
{
        return takes_short(100000);
}

int
short_hex_123456789(void)
{
        return takes_short(0x123456789);
}

int
unsigned_short_0(void)
{
        return takes_unsigned_short(0);
}

int
unsigned_short_1(void)
{
        return takes_unsigned_short(1);
}

int
unsigned_short_minus_1(void)
{
        return takes_unsigned_short(-1);
}

int
unsigned_short_255(void)
{
        return takes_unsigned_short(255);
}

int
unsigned_short_300(void)
{
        return takes_unsigned_short(300);
}

int
unsigned_short_100000(void)
{
        return takes_unsigned_short(100000);
}

int
unsigned_short_hex_123456789(void)
{
        return takes_unsigned_short(0x123456789);
}

int
int_0(void)
{
        return takes_int(0);
}

int
int_1(void)
{
        return takes_int(1);
}

int
int_minus_1(void)
{
        return takes_int(-1);
}

int
int_255(void)
{
        return takes_int(255);
}

int
int_300(void)
{
        return takes_int(300);
}

int
int_65535(void)
{
        return takes_int(65535);
}

int
int_100000(void)
{
        return takes_int(100000);
}

int
int_2147483647(void)
{
        return takes_int(2147483647);
}

int
int_minus_2147483648(void)
{
        return takes_int(-2147483648);
}

int
int_hex_123456789(void)
{
        return takes_int(0x123456789);
}

int
long_0(void)
{
        return takes_long(0);
}

int
long_1(void)
{
        return takes_long(1);
}

int
long_minus_1(void)
{
        return takes_long(-1);
}

int
long_255(void)
{
        return takes_long(255);
}

int
long_300(void)
{
        return takes_long(300);
}

int
long_65535(void)
{
        return takes_long(65535);
}

int
long_100000(void)
{
        return takes_long(100000);
}

int
long_2147483647(void)
{
        return takes_long(2147483647);
}

int
long_minus_2147483648(void)
{
        return takes_long(-2147483648);
}

int
long_4294967295(void)
{
        return takes_long(4294967295);
}

int
long_4294967296(void)
{
        return takes_long(4294967296);
}

int
long_hex_123456789(void)
{
        return takes_long(0x123456789);
}

int
long_2147483648(void)
{
        return takes_long(2147483648);
}

int
long_minus_2147483649(void)
{
        return takes_long(-2147483649);
}

int
long_hex_8000000000000000(void)
{
        return takes_long(0x8000000000000000);
}
