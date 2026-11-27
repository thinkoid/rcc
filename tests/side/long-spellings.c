/* The spellings of int and long, 6.7.3.1p2, each as a local's type, a
   function's result type and a cast's type name.  The order of the
   keywords does not matter, and long long is as wide as long on LP64,
   so every spelling of long writes the same listing: movq loads and
   stores, ldr x and str x; and every spelling of int an int's. */

int
int0(void)
{
        int x = 0;
        return (int) x;
}

signed
int1(void)
{
        signed x = 1;
        return (signed) x;
}

signed int
int2(void)
{
        signed int x = 2;
        return (signed int) x;
}

int signed
int3(void)
{
        int signed x = 3;
        return (int signed) x;
}

long
long4(void)
{
        long x = 4;
        return (long) x;
}

long int
long5(void)
{
        long int x = 5;
        return (long int) x;
}

int long
long6(void)
{
        int long x = 6;
        return (int long) x;
}

signed long
long7(void)
{
        signed long x = 7;
        return (signed long) x;
}

long signed
long8(void)
{
        long signed x = 8;
        return (long signed) x;
}

signed long int
long9(void)
{
        signed long int x = 9;
        return (signed long int) x;
}

long signed int
long10(void)
{
        long signed int x = 10;
        return (long signed int) x;
}

int long signed
long11(void)
{
        int long signed x = 11;
        return (int long signed) x;
}

long long
long12(void)
{
        long long x = 12;
        return (long long) x;
}

long long int
long13(void)
{
        long long int x = 13;
        return (long long int) x;
}

long int long
long14(void)
{
        long int long x = 14;
        return (long int long) x;
}

int long long
long15(void)
{
        int long long x = 15;
        return (int long long) x;
}

signed long long
long16(void)
{
        signed long long x = 16;
        return (signed long long) x;
}

long long signed
long17(void)
{
        long long signed x = 17;
        return (long long signed) x;
}

long signed long
long18(void)
{
        long signed long x = 18;
        return (long signed long) x;
}

signed long long int
long19(void)
{
        signed long long int x = 19;
        return (signed long long int) x;
}
