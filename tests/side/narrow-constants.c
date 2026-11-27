/* The narrow constants, A3.2 and A3.3 of the survey: a constant
   converted to a narrow type is the constant of that type, its low bits
   read by its sign, so (char) 300 is 44, a short's 70000 is 4464, and an
   unsigned char's -1 is 255; stored by one mov of the slot's width on
   x86-64, the value written signed at that width, movb $-1, -1(%rbp)
   for the 255, and 0 by strb wzr or strh wzr on aarch64.  In a register
   it is its promotion: movl $-1, %eax for a char 255 on x86-64 and mov
   w0, #255 on aarch64, where plain char is unsigned.  A constant widened
   to an unsigned short is its low bits zero-extended, (unsigned short)
   (signed char) 128 being 65408. */
int
stores(void)
{
        char c;
        short s;
        unsigned char x;
        signed char y;
        unsigned short z;
        c = 300;
        s = 70000;
        x = -1;
        y = 255;
        z = -128;
        c = 0;
        s = 0;
        c = -129;
        c = 100000;
        s = 32768;
        return 0;
}

int
initializers(void)
{
        char c = 300;
        short s = 70000;
        unsigned char x = -1;
        unsigned short z = 0;
        return 0;
}

char
returned_char(void)
{
        return 255;
}

unsigned char
returned_unsigned_char(void)
{
        return -1;
}

short
returned_short(void)
{
        return 100000;
}

unsigned short
returned_unsigned_short(void)
{
        return -1;
}

signed char
returned_signed_char(void)
{
        return 128;
}

char
used(void)
{
        char x;
        return x = 255;
}

int
chain(void)
{
        char x;
        char y;
        x = y = 0x123456789;
        return 0;
}

long
casts(void)
{
        return (char) -1 + (unsigned char) -1 + (short) 32768 +
               (unsigned short) -1 + (signed char) 200 + (long) (char) 128;
}

short
store_reloaded(void)
{
        short s;
        s = 70000;
        return s;
}

unsigned short
widened_to_unsigned_short(void)
{
        return (unsigned short) (signed char) 128 + (unsigned short) (char) -1;
}
