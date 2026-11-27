/* Two declarations of one function whose types are spelt two ways
   and are of one kind, which agree, 6.7.3.1p2: a parameter, then a
   result. */
int p0(long x);

int
p0(long int x)
{
        return 0;
}

int p1(long x);

int
p1(signed long x)
{
        return 0;
}

int p2(signed long long x);

int
p2(long long x)
{
        return 0;
}

int p3(long long x);

int
p3(long long signed int x)
{
        return 0;
}

int p4(unsigned long x);

int
p4(long unsigned int x)
{
        return 0;
}

int p5(unsigned long long x);

int
p5(long long unsigned x)
{
        return 0;
}

int p6(signed char x);

int
p6(char signed x)
{
        return 0;
}

int p7(int x);

int
p7(signed x)
{
        return 0;
}

int p8(int x);

int
p8(signed int x)
{
        return 0;
}

int p9(unsigned x);

int
p9(unsigned int x)
{
        return 0;
}

int p10(short x);

int
p10(short int x)
{
        return 0;
}

int p11(short x);

int
p11(signed short int x)
{
        return 0;
}

int p12(unsigned short x);

int
p12(short unsigned x)
{
        return 0;
}

long r0(void);

long int
r0(void)
{
        return 0;
}

long r1(void);

signed long
r1(void)
{
        return 0;
}

signed long long r2(void);

long long
r2(void)
{
        return 0;
}

long long r3(void);

long long signed int
r3(void)
{
        return 0;
}

unsigned long r4(void);

long unsigned int
r4(void)
{
        return 0;
}

unsigned long long r5(void);

long long unsigned
r5(void)
{
        return 0;
}

signed char r6(void);

char signed
r6(void)
{
        return 0;
}

int r7(void);

signed
r7(void)
{
        return 0;
}

int r8(void);

signed int
r8(void)
{
        return 0;
}

unsigned r9(void);

unsigned int
r9(void)
{
        return 0;
}

short r10(void);

short int
r10(void)
{
        return 0;
}

short r11(void);

signed short int
r11(void)
{
        return 0;
}

unsigned short r12(void);

short unsigned
r12(void)
{
        return 0;
}
