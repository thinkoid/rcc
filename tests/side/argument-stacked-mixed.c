/* Five arguments of mixed types past the registers on each target,
   from locals and from constants: on x86-64 the 64-bit ones pushed
   from their slots, the others through rax, last first; on aarch64
   each at [sp, #8k] at its own width, in order. */
int mixed7(int a1,
           int a2,
           int a3,
           int a4,
           int a5,
           int a6,
           char p7,
           long p8,
           short p9,
           unsigned char p10,
           long p11);
int mixed9(int a1,
           int a2,
           int a3,
           int a4,
           int a5,
           int a6,
           int a7,
           int a8,
           char p9,
           long p10,
           short p11,
           unsigned char p12,
           long p13);

int
locals7(void)
{
        char c;
        long l;
        short s;
        unsigned char u;
        return mixed7(0, 0, 0, 0, 0, 0, c, l, s, u, l);
}

int
constants7(void)
{
        return mixed7(0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 0x123456789);
}

int
locals9(void)
{
        char c;
        long l;
        short s;
        unsigned char u;
        return mixed9(0, 0, 0, 0, 0, 0, 0, 0, c, l, s, u, l);
}

int
constants9(void)
{
        return mixed9(0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 3, 4, 0x123456789);
}
