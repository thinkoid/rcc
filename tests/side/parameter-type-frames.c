/* The frames of functions of typed parameters: the slots count by
   their types' sizes and alignments, rounded up to 16, with no subq in
   a leaf of up to 128 bytes on x86-64; on aarch64 a parameter past the
   eighth has its slot too, on x86-64 none. */
int g(void);

int
frame0(char a)
{
        return 0;
}

int
frame1(char a, short b)
{
        return 0;
}

int
frame2(char a)
{
        return g();
}

int
frame3(char a, short b)
{
        return g();
}

int
frame4(char a, long b)
{
        return g();
}

int
frame5(long a, char b)
{
        return g();
}

int
frame6(char a, char b, char c, char d, char e, char h)
{
        return g();
}

int
frame7(long a, long b, long c, long d, long e, long h)
{
        return g();
}

int
frame8(int a1,
       int a2,
       int a3,
       int a4,
       int a5,
       int a6,
       int a7,
       int a8,
       char i,
       long j)
{
        return g();
}

int
frame9(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, char i)
{
        return g();
}

int
frame10(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, long i)
{
        return g();
}

int
frame11(long a, long b, long c, long d, long e, long h)
{
        long l1, l2, l3, l4, l5, l6, l7, l8, l9, l10;
        l10 = 1;
        return 0;
}

int
frame12(long a, long b, long c, long d, long e, long h)
{
        long l1, l2, l3, l4, l5, l6, l7, l8, l9, l10;
        char z;
        z = 1;
        return 0;
}

int
frame13(char a,
        long b,
        short c,
        int d,
        unsigned char e,
        unsigned long f,
        short g,
        char h,
        long i)
{
        return 0;
}

int
frame14(int a1,
        int a2,
        int a3,
        int a4,
        int a5,
        int a6,
        char g,
        long h,
        short i,
        unsigned char j,
        long k)
{
        return 0;
}

int
frame15(int a1,
        int a2,
        int a3,
        int a4,
        int a5,
        int a6,
        int a7,
        int a8,
        char i,
        short j,
        long k,
        unsigned char l)
{
        return 0;
}

int
frame16(int a1,
        int a2,
        int a3,
        int a4,
        int a5,
        int a6,
        int a7,
        int a8,
        long i,
        char j,
        short k,
        int l)
{
        return 0;
}
