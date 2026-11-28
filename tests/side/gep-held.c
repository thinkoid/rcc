/* A place through a sum held across its right operand, at each
   stride: across a call x86-64 sums the base and the index, by addq at
   a stride of 1 and by leaq at 2, 4 and 8, and aarch64 by the add of
   the index's form; across a division and a shift the base and the
   index stay in their registers; across && both are spilled. */
int g(void);
void
char_call(char *p, int i)
{
        p[i] = g();
        p[5000] = g();
}

char
char_call_value(char *p, int i)
{
        return p[i] = g();
}

void
char_division(char *p, int i, int a, int b)
{
        p[i] = a / b;
        p[i] = a % b;
}

void
char_shift(char *p, int i, int a, int b)
{
        p[i] = a << b;
}

void
char_logical(char *p, int i, int a, int b)
{
        p[i] = a && b;
}

void
char_sum(char *p, int i, int a)
{
        *(p + i + 1) = g() + a;
}

void
signed_char_call(signed char *p, int i)
{
        p[i] = g();
        p[5000] = g();
}

signed char
signed_char_call_value(signed char *p, int i)
{
        return p[i] = g();
}

void
signed_char_division(signed char *p, int i, int a, int b)
{
        p[i] = a / b;
        p[i] = a % b;
}

void
signed_char_shift(signed char *p, int i, int a, int b)
{
        p[i] = a << b;
}

void
signed_char_logical(signed char *p, int i, int a, int b)
{
        p[i] = a && b;
}

void
signed_char_sum(signed char *p, int i, int a)
{
        *(p + i + 1) = g() + a;
}

void
unsigned_char_call(unsigned char *p, int i)
{
        p[i] = g();
        p[5000] = g();
}

unsigned char
unsigned_char_call_value(unsigned char *p, int i)
{
        return p[i] = g();
}

void
unsigned_char_division(unsigned char *p, int i, int a, int b)
{
        p[i] = a / b;
        p[i] = a % b;
}

void
unsigned_char_shift(unsigned char *p, int i, int a, int b)
{
        p[i] = a << b;
}

void
unsigned_char_logical(unsigned char *p, int i, int a, int b)
{
        p[i] = a && b;
}

void
unsigned_char_sum(unsigned char *p, int i, int a)
{
        *(p + i + 1) = g() + a;
}

void
short_call(short *p, int i)
{
        p[i] = g();
        p[5000] = g();
}

short
short_call_value(short *p, int i)
{
        return p[i] = g();
}

void
short_division(short *p, int i, int a, int b)
{
        p[i] = a / b;
        p[i] = a % b;
}

void
short_shift(short *p, int i, int a, int b)
{
        p[i] = a << b;
}

void
short_logical(short *p, int i, int a, int b)
{
        p[i] = a && b;
}

void
short_sum(short *p, int i, int a)
{
        *(p + i + 1) = g() + a;
}

void
unsigned_short_call(unsigned short *p, int i)
{
        p[i] = g();
        p[5000] = g();
}

unsigned short
unsigned_short_call_value(unsigned short *p, int i)
{
        return p[i] = g();
}

void
unsigned_short_division(unsigned short *p, int i, int a, int b)
{
        p[i] = a / b;
        p[i] = a % b;
}

void
unsigned_short_shift(unsigned short *p, int i, int a, int b)
{
        p[i] = a << b;
}

void
unsigned_short_logical(unsigned short *p, int i, int a, int b)
{
        p[i] = a && b;
}

void
unsigned_short_sum(unsigned short *p, int i, int a)
{
        *(p + i + 1) = g() + a;
}

void
unsigned_call(unsigned *p, int i)
{
        p[i] = g();
        p[5000] = g();
}

unsigned
unsigned_call_value(unsigned *p, int i)
{
        return p[i] = g();
}

void
unsigned_division(unsigned *p, int i, int a, int b)
{
        p[i] = a / b;
        p[i] = a % b;
}

void
unsigned_shift(unsigned *p, int i, int a, int b)
{
        p[i] = a << b;
}

void
unsigned_logical(unsigned *p, int i, int a, int b)
{
        p[i] = a && b;
}

void
unsigned_sum(unsigned *p, int i, int a)
{
        *(p + i + 1) = g() + a;
}

void
long_call(long *p, int i)
{
        p[i] = g();
        p[5000] = g();
}

long
long_call_value(long *p, int i)
{
        return p[i] = g();
}

void
long_division(long *p, int i, int a, int b)
{
        p[i] = a / b;
        p[i] = a % b;
}

void
long_shift(long *p, int i, int a, int b)
{
        p[i] = a << b;
}

void
long_logical(long *p, int i, int a, int b)
{
        p[i] = a && b;
}

void
long_sum(long *p, int i, int a)
{
        *(p + i + 1) = g() + a;
}

void
unsigned_long_call(unsigned long *p, int i)
{
        p[i] = g();
        p[5000] = g();
}

unsigned long
unsigned_long_call_value(unsigned long *p, int i)
{
        return p[i] = g();
}

void
unsigned_long_division(unsigned long *p, int i, int a, int b)
{
        p[i] = a / b;
        p[i] = a % b;
}

void
unsigned_long_shift(unsigned long *p, int i, int a, int b)
{
        p[i] = a << b;
}

void
unsigned_long_logical(unsigned long *p, int i, int a, int b)
{
        p[i] = a && b;
}

void
unsigned_long_sum(unsigned long *p, int i, int a)
{
        *(p + i + 1) = g() + a;
}

void
long_long_call(long long *p, int i)
{
        p[i] = g();
        p[5000] = g();
}

long long
long_long_call_value(long long *p, int i)
{
        return p[i] = g();
}

void
long_long_division(long long *p, int i, int a, int b)
{
        p[i] = a / b;
        p[i] = a % b;
}

void
long_long_shift(long long *p, int i, int a, int b)
{
        p[i] = a << b;
}

void
long_long_logical(long long *p, int i, int a, int b)
{
        p[i] = a && b;
}

void
long_long_sum(long long *p, int i, int a)
{
        *(p + i + 1) = g() + a;
}

void
unsigned_long_long_call(unsigned long long *p, int i)
{
        p[i] = g();
        p[5000] = g();
}

unsigned long long
unsigned_long_long_call_value(unsigned long long *p, int i)
{
        return p[i] = g();
}

void
unsigned_long_long_division(unsigned long long *p, int i, int a, int b)
{
        p[i] = a / b;
        p[i] = a % b;
}

void
unsigned_long_long_shift(unsigned long long *p, int i, int a, int b)
{
        p[i] = a << b;
}

void
unsigned_long_long_logical(unsigned long long *p, int i, int a, int b)
{
        p[i] = a && b;
}

void
unsigned_long_long_sum(unsigned long long *p, int i, int a)
{
        *(p + i + 1) = g() + a;
}
