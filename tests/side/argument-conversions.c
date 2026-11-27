/* An argument of each type to a parameter of each type, 6.5.3.3p6:
   the local's load, its conversion as if by assignment to the
   parameter's type, the store to the argument's slot of the
   parameter's size at its width, movb %al, -2(%rbp), then the load of
   the parameter's type into its register, a narrow one extended to 32
   bits by its sign, movsbl -2(%rbp), %edi and ldrsb w0, [x29, #17]. */
int takes_char(char x);
int takes_signed_char(signed char x);
int takes_unsigned_char(unsigned char x);
int takes_short(short x);
int takes_unsigned_short(unsigned short x);
int takes_int(int x);
int takes_long(long x);

int
char_to_char(void)
{
        char a;
        return takes_char(a);
}

int
char_to_signed_char(void)
{
        char a;
        return takes_signed_char(a);
}

int
char_to_unsigned_char(void)
{
        char a;
        return takes_unsigned_char(a);
}

int
char_to_short(void)
{
        char a;
        return takes_short(a);
}

int
char_to_unsigned_short(void)
{
        char a;
        return takes_unsigned_short(a);
}

int
char_to_int(void)
{
        char a;
        return takes_int(a);
}

int
char_to_long(void)
{
        char a;
        return takes_long(a);
}

int
signed_char_to_char(void)
{
        signed char a;
        return takes_char(a);
}

int
signed_char_to_signed_char(void)
{
        signed char a;
        return takes_signed_char(a);
}

int
signed_char_to_unsigned_char(void)
{
        signed char a;
        return takes_unsigned_char(a);
}

int
signed_char_to_short(void)
{
        signed char a;
        return takes_short(a);
}

int
signed_char_to_unsigned_short(void)
{
        signed char a;
        return takes_unsigned_short(a);
}

int
signed_char_to_int(void)
{
        signed char a;
        return takes_int(a);
}

int
signed_char_to_long(void)
{
        signed char a;
        return takes_long(a);
}

int
unsigned_char_to_char(void)
{
        unsigned char a;
        return takes_char(a);
}

int
unsigned_char_to_signed_char(void)
{
        unsigned char a;
        return takes_signed_char(a);
}

int
unsigned_char_to_unsigned_char(void)
{
        unsigned char a;
        return takes_unsigned_char(a);
}

int
unsigned_char_to_short(void)
{
        unsigned char a;
        return takes_short(a);
}

int
unsigned_char_to_unsigned_short(void)
{
        unsigned char a;
        return takes_unsigned_short(a);
}

int
unsigned_char_to_int(void)
{
        unsigned char a;
        return takes_int(a);
}

int
unsigned_char_to_long(void)
{
        unsigned char a;
        return takes_long(a);
}

int
short_to_char(void)
{
        short a;
        return takes_char(a);
}

int
short_to_signed_char(void)
{
        short a;
        return takes_signed_char(a);
}

int
short_to_unsigned_char(void)
{
        short a;
        return takes_unsigned_char(a);
}

int
short_to_short(void)
{
        short a;
        return takes_short(a);
}

int
short_to_unsigned_short(void)
{
        short a;
        return takes_unsigned_short(a);
}

int
short_to_int(void)
{
        short a;
        return takes_int(a);
}

int
short_to_long(void)
{
        short a;
        return takes_long(a);
}

int
unsigned_short_to_char(void)
{
        unsigned short a;
        return takes_char(a);
}

int
unsigned_short_to_signed_char(void)
{
        unsigned short a;
        return takes_signed_char(a);
}

int
unsigned_short_to_unsigned_char(void)
{
        unsigned short a;
        return takes_unsigned_char(a);
}

int
unsigned_short_to_short(void)
{
        unsigned short a;
        return takes_short(a);
}

int
unsigned_short_to_unsigned_short(void)
{
        unsigned short a;
        return takes_unsigned_short(a);
}

int
unsigned_short_to_int(void)
{
        unsigned short a;
        return takes_int(a);
}

int
unsigned_short_to_long(void)
{
        unsigned short a;
        return takes_long(a);
}

int
int_to_char(void)
{
        int a;
        return takes_char(a);
}

int
int_to_signed_char(void)
{
        int a;
        return takes_signed_char(a);
}

int
int_to_unsigned_char(void)
{
        int a;
        return takes_unsigned_char(a);
}

int
int_to_short(void)
{
        int a;
        return takes_short(a);
}

int
int_to_unsigned_short(void)
{
        int a;
        return takes_unsigned_short(a);
}

int
int_to_int(void)
{
        int a;
        return takes_int(a);
}

int
int_to_long(void)
{
        int a;
        return takes_long(a);
}

int
unsigned_to_char(void)
{
        unsigned a;
        return takes_char(a);
}

int
unsigned_to_signed_char(void)
{
        unsigned a;
        return takes_signed_char(a);
}

int
unsigned_to_unsigned_char(void)
{
        unsigned a;
        return takes_unsigned_char(a);
}

int
unsigned_to_short(void)
{
        unsigned a;
        return takes_short(a);
}

int
unsigned_to_unsigned_short(void)
{
        unsigned a;
        return takes_unsigned_short(a);
}

int
unsigned_to_int(void)
{
        unsigned a;
        return takes_int(a);
}

int
unsigned_to_long(void)
{
        unsigned a;
        return takes_long(a);
}

int
long_to_char(void)
{
        long a;
        return takes_char(a);
}

int
long_to_signed_char(void)
{
        long a;
        return takes_signed_char(a);
}

int
long_to_unsigned_char(void)
{
        long a;
        return takes_unsigned_char(a);
}

int
long_to_short(void)
{
        long a;
        return takes_short(a);
}

int
long_to_unsigned_short(void)
{
        long a;
        return takes_unsigned_short(a);
}

int
long_to_int(void)
{
        long a;
        return takes_int(a);
}

int
long_to_long(void)
{
        long a;
        return takes_long(a);
}
