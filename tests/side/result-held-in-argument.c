/* A value held across a call inside an argument: k's two slots of
   the type's size, the spill after them at the next 8-aligned
   offset. */
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
int k_char(char a, char b);
int k_signed_char(signed char a, signed char b);
int k_unsigned_char(unsigned char a, unsigned char b);
int k_short(short a, short b);
int k_unsigned_short(unsigned short a, unsigned short b);
int k_int(int a, int b);
int k_unsigned(unsigned a, unsigned b);
int k_long(long a, long b);
int k_unsigned_long(unsigned long a, unsigned long b);
int k_long_long(long long a, long long b);
int k_unsigned_long_long(unsigned long long a, unsigned long long b);

int
in_argument_char(void)
{
        char x;
        return k_char(x, x + result_char());
}

int
in_argument_signed_char(void)
{
        signed char x;
        return k_signed_char(x, x + result_signed_char());
}

int
in_argument_unsigned_char(void)
{
        unsigned char x;
        return k_unsigned_char(x, x + result_unsigned_char());
}

int
in_argument_short(void)
{
        short x;
        return k_short(x, x + result_short());
}

int
in_argument_unsigned_short(void)
{
        unsigned short x;
        return k_unsigned_short(x, x + result_unsigned_short());
}

int
in_argument_int(void)
{
        int x;
        return k_int(x, x + result_int());
}

int
in_argument_unsigned(void)
{
        unsigned x;
        return k_unsigned(x, x + result_unsigned());
}

int
in_argument_long(void)
{
        long x;
        return k_long(x, x + result_long());
}

int
in_argument_unsigned_long(void)
{
        unsigned long x;
        return k_unsigned_long(x, x + result_unsigned_long());
}

int
in_argument_long_long(void)
{
        long long x;
        return k_long_long(x, x + result_long_long());
}

int
in_argument_unsigned_long_long(void)
{
        unsigned long long x;
        return k_unsigned_long_long(x, x + result_unsigned_long_long());
}
