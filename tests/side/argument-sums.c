/* A sum of two locals of each type as an argument of each type: the
   sum at the common type, its conversion to the parameter's, the store
   and the load as for a local, movslq %eax, %rax and sxtw x0, w0 for
   an int sum widened to a long. */
int takes_char(char x);
int takes_signed_char(signed char x);
int takes_unsigned_char(unsigned char x);
int takes_short(short x);
int takes_unsigned_short(unsigned short x);
int takes_int(int x);
int takes_long(long x);

int
char_sum_to_char(void)
{
        char a;
        char b;
        return takes_char(a + b);
}

int
char_sum_to_signed_char(void)
{
        char a;
        char b;
        return takes_signed_char(a + b);
}

int
char_sum_to_unsigned_char(void)
{
        char a;
        char b;
        return takes_unsigned_char(a + b);
}

int
char_sum_to_short(void)
{
        char a;
        char b;
        return takes_short(a + b);
}

int
char_sum_to_unsigned_short(void)
{
        char a;
        char b;
        return takes_unsigned_short(a + b);
}

int
char_sum_to_int(void)
{
        char a;
        char b;
        return takes_int(a + b);
}

int
char_sum_to_long(void)
{
        char a;
        char b;
        return takes_long(a + b);
}

int
signed_char_sum_to_char(void)
{
        signed char a;
        signed char b;
        return takes_char(a + b);
}

int
signed_char_sum_to_signed_char(void)
{
        signed char a;
        signed char b;
        return takes_signed_char(a + b);
}

int
signed_char_sum_to_unsigned_char(void)
{
        signed char a;
        signed char b;
        return takes_unsigned_char(a + b);
}

int
signed_char_sum_to_short(void)
{
        signed char a;
        signed char b;
        return takes_short(a + b);
}

int
signed_char_sum_to_unsigned_short(void)
{
        signed char a;
        signed char b;
        return takes_unsigned_short(a + b);
}

int
signed_char_sum_to_int(void)
{
        signed char a;
        signed char b;
        return takes_int(a + b);
}

int
signed_char_sum_to_long(void)
{
        signed char a;
        signed char b;
        return takes_long(a + b);
}

int
unsigned_char_sum_to_char(void)
{
        unsigned char a;
        unsigned char b;
        return takes_char(a + b);
}

int
unsigned_char_sum_to_signed_char(void)
{
        unsigned char a;
        unsigned char b;
        return takes_signed_char(a + b);
}

int
unsigned_char_sum_to_unsigned_char(void)
{
        unsigned char a;
        unsigned char b;
        return takes_unsigned_char(a + b);
}

int
unsigned_char_sum_to_short(void)
{
        unsigned char a;
        unsigned char b;
        return takes_short(a + b);
}

int
unsigned_char_sum_to_unsigned_short(void)
{
        unsigned char a;
        unsigned char b;
        return takes_unsigned_short(a + b);
}

int
unsigned_char_sum_to_int(void)
{
        unsigned char a;
        unsigned char b;
        return takes_int(a + b);
}

int
unsigned_char_sum_to_long(void)
{
        unsigned char a;
        unsigned char b;
        return takes_long(a + b);
}

int
short_sum_to_char(void)
{
        short a;
        short b;
        return takes_char(a + b);
}

int
short_sum_to_signed_char(void)
{
        short a;
        short b;
        return takes_signed_char(a + b);
}

int
short_sum_to_unsigned_char(void)
{
        short a;
        short b;
        return takes_unsigned_char(a + b);
}

int
short_sum_to_short(void)
{
        short a;
        short b;
        return takes_short(a + b);
}

int
short_sum_to_unsigned_short(void)
{
        short a;
        short b;
        return takes_unsigned_short(a + b);
}

int
short_sum_to_int(void)
{
        short a;
        short b;
        return takes_int(a + b);
}

int
short_sum_to_long(void)
{
        short a;
        short b;
        return takes_long(a + b);
}

int
unsigned_short_sum_to_char(void)
{
        unsigned short a;
        unsigned short b;
        return takes_char(a + b);
}

int
unsigned_short_sum_to_signed_char(void)
{
        unsigned short a;
        unsigned short b;
        return takes_signed_char(a + b);
}

int
unsigned_short_sum_to_unsigned_char(void)
{
        unsigned short a;
        unsigned short b;
        return takes_unsigned_char(a + b);
}

int
unsigned_short_sum_to_short(void)
{
        unsigned short a;
        unsigned short b;
        return takes_short(a + b);
}

int
unsigned_short_sum_to_unsigned_short(void)
{
        unsigned short a;
        unsigned short b;
        return takes_unsigned_short(a + b);
}

int
unsigned_short_sum_to_int(void)
{
        unsigned short a;
        unsigned short b;
        return takes_int(a + b);
}

int
unsigned_short_sum_to_long(void)
{
        unsigned short a;
        unsigned short b;
        return takes_long(a + b);
}

int
int_sum_to_char(void)
{
        int a;
        int b;
        return takes_char(a + b);
}

int
int_sum_to_signed_char(void)
{
        int a;
        int b;
        return takes_signed_char(a + b);
}

int
int_sum_to_unsigned_char(void)
{
        int a;
        int b;
        return takes_unsigned_char(a + b);
}

int
int_sum_to_short(void)
{
        int a;
        int b;
        return takes_short(a + b);
}

int
int_sum_to_unsigned_short(void)
{
        int a;
        int b;
        return takes_unsigned_short(a + b);
}

int
int_sum_to_int(void)
{
        int a;
        int b;
        return takes_int(a + b);
}

int
int_sum_to_long(void)
{
        int a;
        int b;
        return takes_long(a + b);
}

int
unsigned_sum_to_char(void)
{
        unsigned a;
        unsigned b;
        return takes_char(a + b);
}

int
unsigned_sum_to_signed_char(void)
{
        unsigned a;
        unsigned b;
        return takes_signed_char(a + b);
}

int
unsigned_sum_to_unsigned_char(void)
{
        unsigned a;
        unsigned b;
        return takes_unsigned_char(a + b);
}

int
unsigned_sum_to_short(void)
{
        unsigned a;
        unsigned b;
        return takes_short(a + b);
}

int
unsigned_sum_to_unsigned_short(void)
{
        unsigned a;
        unsigned b;
        return takes_unsigned_short(a + b);
}

int
unsigned_sum_to_int(void)
{
        unsigned a;
        unsigned b;
        return takes_int(a + b);
}

int
unsigned_sum_to_long(void)
{
        unsigned a;
        unsigned b;
        return takes_long(a + b);
}

int
long_sum_to_char(void)
{
        long a;
        long b;
        return takes_char(a + b);
}

int
long_sum_to_signed_char(void)
{
        long a;
        long b;
        return takes_signed_char(a + b);
}

int
long_sum_to_unsigned_char(void)
{
        long a;
        long b;
        return takes_unsigned_char(a + b);
}

int
long_sum_to_short(void)
{
        long a;
        long b;
        return takes_short(a + b);
}

int
long_sum_to_unsigned_short(void)
{
        long a;
        long b;
        return takes_unsigned_short(a + b);
}

int
long_sum_to_int(void)
{
        long a;
        long b;
        return takes_int(a + b);
}

int
long_sum_to_long(void)
{
        long a;
        long b;
        return takes_long(a + b);
}
