/* A call's result of each type as the initializer of a local of each
   type, and assigned to one: the conversion, then the store at the
   local's width; a widening to 16 bits is extended to 32 first,
   movsbl %al, %eax and movw %ax, -2(%rbp). */
char result_char(void);
signed char result_signed_char(void);
unsigned char result_unsigned_char(void);
short result_short(void);
unsigned short result_unsigned_short(void);
int result_int(void);
unsigned result_unsigned(void);
long result_long(void);
unsigned long result_unsigned_long(void);

int
char_initializes_char(void)
{
        char x = result_char();
        return 0;
}

int
char_initializes_signed_char(void)
{
        signed char x = result_char();
        return 0;
}

int
char_initializes_unsigned_char(void)
{
        unsigned char x = result_char();
        return 0;
}

int
char_initializes_short(void)
{
        short x = result_char();
        return 0;
}

int
char_initializes_unsigned_short(void)
{
        unsigned short x = result_char();
        return 0;
}

int
char_initializes_int(void)
{
        int x = result_char();
        return 0;
}

int
char_initializes_unsigned(void)
{
        unsigned x = result_char();
        return 0;
}

int
char_initializes_long(void)
{
        long x = result_char();
        return 0;
}

int
char_initializes_unsigned_long(void)
{
        unsigned long x = result_char();
        return 0;
}

int
signed_char_initializes_char(void)
{
        char x = result_signed_char();
        return 0;
}

int
signed_char_initializes_signed_char(void)
{
        signed char x = result_signed_char();
        return 0;
}

int
signed_char_initializes_unsigned_char(void)
{
        unsigned char x = result_signed_char();
        return 0;
}

int
signed_char_initializes_short(void)
{
        short x = result_signed_char();
        return 0;
}

int
signed_char_initializes_unsigned_short(void)
{
        unsigned short x = result_signed_char();
        return 0;
}

int
signed_char_initializes_int(void)
{
        int x = result_signed_char();
        return 0;
}

int
signed_char_initializes_unsigned(void)
{
        unsigned x = result_signed_char();
        return 0;
}

int
signed_char_initializes_long(void)
{
        long x = result_signed_char();
        return 0;
}

int
signed_char_initializes_unsigned_long(void)
{
        unsigned long x = result_signed_char();
        return 0;
}

int
unsigned_char_initializes_char(void)
{
        char x = result_unsigned_char();
        return 0;
}

int
unsigned_char_initializes_signed_char(void)
{
        signed char x = result_unsigned_char();
        return 0;
}

int
unsigned_char_initializes_unsigned_char(void)
{
        unsigned char x = result_unsigned_char();
        return 0;
}

int
unsigned_char_initializes_short(void)
{
        short x = result_unsigned_char();
        return 0;
}

int
unsigned_char_initializes_unsigned_short(void)
{
        unsigned short x = result_unsigned_char();
        return 0;
}

int
unsigned_char_initializes_int(void)
{
        int x = result_unsigned_char();
        return 0;
}

int
unsigned_char_initializes_unsigned(void)
{
        unsigned x = result_unsigned_char();
        return 0;
}

int
unsigned_char_initializes_long(void)
{
        long x = result_unsigned_char();
        return 0;
}

int
unsigned_char_initializes_unsigned_long(void)
{
        unsigned long x = result_unsigned_char();
        return 0;
}

int
short_initializes_char(void)
{
        char x = result_short();
        return 0;
}

int
short_initializes_signed_char(void)
{
        signed char x = result_short();
        return 0;
}

int
short_initializes_unsigned_char(void)
{
        unsigned char x = result_short();
        return 0;
}

int
short_initializes_short(void)
{
        short x = result_short();
        return 0;
}

int
short_initializes_unsigned_short(void)
{
        unsigned short x = result_short();
        return 0;
}

int
short_initializes_int(void)
{
        int x = result_short();
        return 0;
}

int
short_initializes_unsigned(void)
{
        unsigned x = result_short();
        return 0;
}

int
short_initializes_long(void)
{
        long x = result_short();
        return 0;
}

int
short_initializes_unsigned_long(void)
{
        unsigned long x = result_short();
        return 0;
}

int
unsigned_short_initializes_char(void)
{
        char x = result_unsigned_short();
        return 0;
}

int
unsigned_short_initializes_signed_char(void)
{
        signed char x = result_unsigned_short();
        return 0;
}

int
unsigned_short_initializes_unsigned_char(void)
{
        unsigned char x = result_unsigned_short();
        return 0;
}

int
unsigned_short_initializes_short(void)
{
        short x = result_unsigned_short();
        return 0;
}

int
unsigned_short_initializes_unsigned_short(void)
{
        unsigned short x = result_unsigned_short();
        return 0;
}

int
unsigned_short_initializes_int(void)
{
        int x = result_unsigned_short();
        return 0;
}

int
unsigned_short_initializes_unsigned(void)
{
        unsigned x = result_unsigned_short();
        return 0;
}

int
unsigned_short_initializes_long(void)
{
        long x = result_unsigned_short();
        return 0;
}

int
unsigned_short_initializes_unsigned_long(void)
{
        unsigned long x = result_unsigned_short();
        return 0;
}

int
int_initializes_char(void)
{
        char x = result_int();
        return 0;
}

int
int_initializes_signed_char(void)
{
        signed char x = result_int();
        return 0;
}

int
int_initializes_unsigned_char(void)
{
        unsigned char x = result_int();
        return 0;
}

int
int_initializes_short(void)
{
        short x = result_int();
        return 0;
}

int
int_initializes_unsigned_short(void)
{
        unsigned short x = result_int();
        return 0;
}

int
int_initializes_int(void)
{
        int x = result_int();
        return 0;
}

int
int_initializes_unsigned(void)
{
        unsigned x = result_int();
        return 0;
}

int
int_initializes_long(void)
{
        long x = result_int();
        return 0;
}

int
int_initializes_unsigned_long(void)
{
        unsigned long x = result_int();
        return 0;
}

int
unsigned_initializes_char(void)
{
        char x = result_unsigned();
        return 0;
}

int
unsigned_initializes_signed_char(void)
{
        signed char x = result_unsigned();
        return 0;
}

int
unsigned_initializes_unsigned_char(void)
{
        unsigned char x = result_unsigned();
        return 0;
}

int
unsigned_initializes_short(void)
{
        short x = result_unsigned();
        return 0;
}

int
unsigned_initializes_unsigned_short(void)
{
        unsigned short x = result_unsigned();
        return 0;
}

int
unsigned_initializes_int(void)
{
        int x = result_unsigned();
        return 0;
}

int
unsigned_initializes_unsigned(void)
{
        unsigned x = result_unsigned();
        return 0;
}

int
unsigned_initializes_long(void)
{
        long x = result_unsigned();
        return 0;
}

int
unsigned_initializes_unsigned_long(void)
{
        unsigned long x = result_unsigned();
        return 0;
}

int
long_initializes_char(void)
{
        char x = result_long();
        return 0;
}

int
long_initializes_signed_char(void)
{
        signed char x = result_long();
        return 0;
}

int
long_initializes_unsigned_char(void)
{
        unsigned char x = result_long();
        return 0;
}

int
long_initializes_short(void)
{
        short x = result_long();
        return 0;
}

int
long_initializes_unsigned_short(void)
{
        unsigned short x = result_long();
        return 0;
}

int
long_initializes_int(void)
{
        int x = result_long();
        return 0;
}

int
long_initializes_unsigned(void)
{
        unsigned x = result_long();
        return 0;
}

int
long_initializes_long(void)
{
        long x = result_long();
        return 0;
}

int
long_initializes_unsigned_long(void)
{
        unsigned long x = result_long();
        return 0;
}

int
unsigned_long_initializes_char(void)
{
        char x = result_unsigned_long();
        return 0;
}

int
unsigned_long_initializes_signed_char(void)
{
        signed char x = result_unsigned_long();
        return 0;
}

int
unsigned_long_initializes_unsigned_char(void)
{
        unsigned char x = result_unsigned_long();
        return 0;
}

int
unsigned_long_initializes_short(void)
{
        short x = result_unsigned_long();
        return 0;
}

int
unsigned_long_initializes_unsigned_short(void)
{
        unsigned short x = result_unsigned_long();
        return 0;
}

int
unsigned_long_initializes_int(void)
{
        int x = result_unsigned_long();
        return 0;
}

int
unsigned_long_initializes_unsigned(void)
{
        unsigned x = result_unsigned_long();
        return 0;
}

int
unsigned_long_initializes_long(void)
{
        long x = result_unsigned_long();
        return 0;
}

int
unsigned_long_initializes_unsigned_long(void)
{
        unsigned long x = result_unsigned_long();
        return 0;
}
