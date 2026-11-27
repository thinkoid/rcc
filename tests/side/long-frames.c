/* The frame of int and long locals, taken in declaration order: a long
   takes 8 bytes at an alignment of 8, past the slots before it, so an
   int before it leaves 4 bytes unused: -4, -16, -20, -32 on x86-64,
   #16, #24, #32, #40 on aarch64, and a spill's 8 bytes after them.  A
   frame past the red zone's 128 bytes moves rsp on x86-64. */
int
mixed(void)
{
        int a;
        long b;
        int c;
        long d;
        a = 1;
        b = 2;
        c = 3;
        d = 4;
        return 0;
}

int
longs_first(void)
{
        long a;
        long b;
        int c;
        int d;
        a = 1;
        b = 2;
        c = 3;
        d = 4;
        return 0;
}

int
sixteen(void)
{
        long a0, a1, a2, a3, a4, a5, a6, a7;
        long b0, b1, b2, b3, b4, b5, b6, b7;
        a0 = 1;
        b7 = 2;
        return 0;
}

int
seventeen(void)
{
        long a0, a1, a2, a3, a4, a5, a6, a7;
        long b0, b1, b2, b3, b4, b5, b6, b7;
        int c;
        a0 = 1;
        c = 2;
        return 0;
}
