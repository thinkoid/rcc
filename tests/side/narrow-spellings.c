/* The spellings of the narrow types, 6.7.3.1p2, each as a local's
   type, a function's result type and a cast's type name.  Plain char is
   signed on x86-64 and unsigned on aarch64, so its load is movsbl there
   and ldrb here; a narrow object is read by its extending load at 32
   bits whatever consumes it, and a narrow result is the promoted value
   in the result register. */

char
n0(void)
{
        char x = 0;
        return (char) x;
}

long
n0_long(void)
{
        char x = 0;
        return x;
}

signed char
n1(void)
{
        signed char x = 1;
        return (signed char) x;
}

long
n1_long(void)
{
        signed char x = 1;
        return x;
}

char signed
n2(void)
{
        char signed x = 2;
        return (char signed) x;
}

long
n2_long(void)
{
        char signed x = 2;
        return x;
}

unsigned char
n3(void)
{
        unsigned char x = 3;
        return (unsigned char) x;
}

long
n3_long(void)
{
        unsigned char x = 3;
        return x;
}

char unsigned
n4(void)
{
        char unsigned x = 4;
        return (char unsigned) x;
}

long
n4_long(void)
{
        char unsigned x = 4;
        return x;
}

short
n5(void)
{
        short x = 5;
        return (short) x;
}

long
n5_long(void)
{
        short x = 5;
        return x;
}

short int
n6(void)
{
        short int x = 6;
        return (short int) x;
}

long
n6_long(void)
{
        short int x = 6;
        return x;
}

int short
n7(void)
{
        int short x = 7;
        return (int short) x;
}

long
n7_long(void)
{
        int short x = 7;
        return x;
}

signed short
n8(void)
{
        signed short x = 8;
        return (signed short) x;
}

long
n8_long(void)
{
        signed short x = 8;
        return x;
}

short signed
n9(void)
{
        short signed x = 9;
        return (short signed) x;
}

long
n9_long(void)
{
        short signed x = 9;
        return x;
}

signed short int
n10(void)
{
        signed short int x = 10;
        return (signed short int) x;
}

long
n10_long(void)
{
        signed short int x = 10;
        return x;
}

int signed short
n11(void)
{
        int signed short x = 11;
        return (int signed short) x;
}

long
n11_long(void)
{
        int signed short x = 11;
        return x;
}

short int signed
n12(void)
{
        short int signed x = 12;
        return (short int signed) x;
}

long
n12_long(void)
{
        short int signed x = 12;
        return x;
}

unsigned short
n13(void)
{
        unsigned short x = 13;
        return (unsigned short) x;
}

long
n13_long(void)
{
        unsigned short x = 13;
        return x;
}

short unsigned
n14(void)
{
        short unsigned x = 14;
        return (short unsigned) x;
}

long
n14_long(void)
{
        short unsigned x = 14;
        return x;
}

unsigned short int
n15(void)
{
        unsigned short int x = 15;
        return (unsigned short int) x;
}

long
n15_long(void)
{
        unsigned short int x = 15;
        return x;
}

int unsigned short
n16(void)
{
        int unsigned short x = 16;
        return (int unsigned short) x;
}

long
n16_long(void)
{
        int unsigned short x = 16;
        return x;
}
