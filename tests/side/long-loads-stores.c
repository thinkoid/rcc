/* The loads, the stores and the constants of int and long, A3 of the
   survey: a long is loaded by movq and stored by movq, ldr x and str x;
   a copy b = a; is the load and the store, and a bare a; the load.  A
   constant stored is one mov of the slot's width on x86-64, movq $-1,
   -8(%rbp), but for one past int's range, movabsq into rax and the
   store; on aarch64 a 0 is str xzr, any other the constant row and the
   store.  A used assignment is the constant in a register and the
   store, and a chain stores the one register twice. */
long
copy(void)
{
        long a;
        long b;
        a = 5;
        b = a;
        return b;
}

int
bare(void)
{
        long a = 1;
        a;
        return 0;
}

int
stores(void)
{
        long x;
        x = 0;
        x = -1;
        x = 2147483647;
        x = 2147483648;
        x = -2147483648;
        x = -2147483649;
        x = 4294967295;
        x = 0x123456789;
        x = 0x5555555555555555;
        x = 0x0000ffff00000000;
        return 0;
}

int
initializers(void)
{
        long a = 0;
        long b = -1;
        long c = 2147483648;
        long d = 0x123456789;
        int e = 7;
        return 0;
}

long
used(void)
{
        long x;
        return x = 0x123456789;
}

long
used_zero(void)
{
        long x;
        return x = 0;
}

int
chain(void)
{
        long x;
        long y;
        x = y = 0;
        x = y = 2147483648;
        return 0;
}

int
store_of_int_value(void)
{
        int a = 3;
        long l;
        l = a;
        return 0;
}

int
store_of_long_value(void)
{
        long l = 3;
        int a;
        a = l;
        return a;
}
