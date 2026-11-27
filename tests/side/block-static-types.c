/* A block's static object of each integer type, initialized and not,
   read, stored and widened: its data as an object's at file scope, its
   accesses a defined object's under its label, and the warning of a
   signed overflow in its initializer. */

long
statics_c(char a)
{
        static char s = 200;
        static char z;
        z = a;
        s = s + z;
        return s;
}

long
statics_sc(signed char a)
{
        static signed char s = -1;
        static signed char z;
        z = a;
        s = s + z;
        return s;
}

long
statics_uc(unsigned char a)
{
        static unsigned char s = 300;
        static unsigned char z;
        z = a;
        s = s + z;
        return s;
}

long
statics_s(short a)
{
        static short s = 70000;
        static short z;
        z = a;
        s = s + z;
        return s;
}

long
statics_us(unsigned short a)
{
        static unsigned short s = -1;
        static unsigned short z;
        z = a;
        s = s + z;
        return s;
}

long
statics_i(int a)
{
        static int s = 2147483647 + 1;
        static int z;
        z = a;
        s = s + z;
        return s;
}

long
statics_u(unsigned a)
{
        static unsigned s = -1;
        static unsigned z;
        z = a;
        s = s + z;
        return s;
}

long
statics_l(long a)
{
        static long s = 9223372036854775807 + 1;
        static long z;
        z = a;
        s = s + z;
        return s;
}

long
statics_ul(unsigned long a)
{
        static unsigned long s = -1;
        static unsigned long z;
        z = a;
        s = s + z;
        return s;
}

long
statics_ll(long long a)
{
        static long long s = 0x123456789;
        static long long z;
        z = a;
        s = s + z;
        return s;
}

long
statics_ull(unsigned long long a)
{
        static unsigned long long s = 0;
        static unsigned long long z;
        z = a;
        s = s + z;
        return s;
}
