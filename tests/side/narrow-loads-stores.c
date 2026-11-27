/* The load, the store and the copy of each narrow type, A3.1 of the
   survey: the store of the register's low bytes at the object's width,
   movb %al, -2(%rbp), strb w0; the load of the object by its own sign
   at 32 bits, movsbl or movzbl, ldrsb or ldrb, whatever consumes it, a
   copy b = a; and a bare a; included. */

char
copy_char(void)
{
        char a;
        char b;
        a = 5;
        b = a;
        return b;
}

int
bare_char(void)
{
        char a = 5;
        a;
        return 0;
}

signed char
copy_signed_char(void)
{
        signed char a;
        signed char b;
        a = 5;
        b = a;
        return b;
}

int
bare_signed_char(void)
{
        signed char a = 5;
        a;
        return 0;
}

unsigned char
copy_unsigned_char(void)
{
        unsigned char a;
        unsigned char b;
        a = 5;
        b = a;
        return b;
}

int
bare_unsigned_char(void)
{
        unsigned char a = 5;
        a;
        return 0;
}

short
copy_short(void)
{
        short a;
        short b;
        a = 5;
        b = a;
        return b;
}

int
bare_short(void)
{
        short a = 5;
        a;
        return 0;
}

unsigned short
copy_unsigned_short(void)
{
        unsigned short a;
        unsigned short b;
        a = 5;
        b = a;
        return b;
}

int
bare_unsigned_short(void)
{
        unsigned short a = 5;
        a;
        return 0;
}
