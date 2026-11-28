/* The index of each integer type, widened to 64 bits in the sign of
   its promoted type: a char, a short and an int signed, movsbq, movswq,
   movslq and sxtw; an unsigned char and an unsigned short too, which
   promote to int, though their loads are valid to 64 bits, x1, lsl #2;
   an unsigned by movl and x1, lsl #2, or uxtw #2 for a call's result;
   a narrow call's result extended first, sxtb w0, w0; on a pointer to
   int, to char and to long. */
char int_char_g(void);

int
int_char_index(int *p, char i)
{
        return p[i] + p[i + 1] + *(p + i) + *(p - i) + i[p];
}

int
int_char_call(int *p)
{
        return p[int_char_g()];
}

void
int_char_written(int *p, char i, int v)
{
        p[i] = v;
}

int *
int_char_values(int *p, char i)
{
        return (p + i) + ((p - i) - p);
}

signed char int_signed_char_g(void);

int
int_signed_char_index(int *p, signed char i)
{
        return p[i] + p[i + 1] + *(p + i) + *(p - i) + i[p];
}

int
int_signed_char_call(int *p)
{
        return p[int_signed_char_g()];
}

void
int_signed_char_written(int *p, signed char i, int v)
{
        p[i] = v;
}

int *
int_signed_char_values(int *p, signed char i)
{
        return (p + i) + ((p - i) - p);
}

unsigned char int_unsigned_char_g(void);

int
int_unsigned_char_index(int *p, unsigned char i)
{
        return p[i] + p[i + 1] + *(p + i) + *(p - i) + i[p];
}

int
int_unsigned_char_call(int *p)
{
        return p[int_unsigned_char_g()];
}

void
int_unsigned_char_written(int *p, unsigned char i, int v)
{
        p[i] = v;
}

int *
int_unsigned_char_values(int *p, unsigned char i)
{
        return (p + i) + ((p - i) - p);
}

short int_short_g(void);

int
int_short_index(int *p, short i)
{
        return p[i] + p[i + 1] + *(p + i) + *(p - i) + i[p];
}

int
int_short_call(int *p)
{
        return p[int_short_g()];
}

void
int_short_written(int *p, short i, int v)
{
        p[i] = v;
}

int *
int_short_values(int *p, short i)
{
        return (p + i) + ((p - i) - p);
}

unsigned short int_unsigned_short_g(void);

int
int_unsigned_short_index(int *p, unsigned short i)
{
        return p[i] + p[i + 1] + *(p + i) + *(p - i) + i[p];
}

int
int_unsigned_short_call(int *p)
{
        return p[int_unsigned_short_g()];
}

void
int_unsigned_short_written(int *p, unsigned short i, int v)
{
        p[i] = v;
}

int *
int_unsigned_short_values(int *p, unsigned short i)
{
        return (p + i) + ((p - i) - p);
}

int int_int_g(void);

int
int_int_index(int *p, int i)
{
        return p[i] + p[i + 1] + *(p + i) + *(p - i) + i[p];
}

int
int_int_call(int *p)
{
        return p[int_int_g()];
}

void
int_int_written(int *p, int i, int v)
{
        p[i] = v;
}

int *
int_int_values(int *p, int i)
{
        return (p + i) + ((p - i) - p);
}

unsigned int_unsigned_g(void);

int
int_unsigned_index(int *p, unsigned i)
{
        return p[i] + p[i + 1] + *(p + i) + *(p - i) + i[p];
}

int
int_unsigned_call(int *p)
{
        return p[int_unsigned_g()];
}

void
int_unsigned_written(int *p, unsigned i, int v)
{
        p[i] = v;
}

int *
int_unsigned_values(int *p, unsigned i)
{
        return (p + i) + ((p - i) - p);
}

long int_long_g(void);

int
int_long_index(int *p, long i)
{
        return p[i] + p[i + 1] + *(p + i) + *(p - i) + i[p];
}

int
int_long_call(int *p)
{
        return p[int_long_g()];
}

void
int_long_written(int *p, long i, int v)
{
        p[i] = v;
}

int *
int_long_values(int *p, long i)
{
        return (p + i) + ((p - i) - p);
}

unsigned long int_unsigned_long_g(void);

int
int_unsigned_long_index(int *p, unsigned long i)
{
        return p[i] + p[i + 1] + *(p + i) + *(p - i) + i[p];
}

int
int_unsigned_long_call(int *p)
{
        return p[int_unsigned_long_g()];
}

void
int_unsigned_long_written(int *p, unsigned long i, int v)
{
        p[i] = v;
}

int *
int_unsigned_long_values(int *p, unsigned long i)
{
        return (p + i) + ((p - i) - p);
}

char char_char_g(void);

char
char_char_index(char *p, char i)
{
        return p[i] + p[i + 1] + *(p + i) + *(p - i) + i[p];
}

char
char_char_call(char *p)
{
        return p[char_char_g()];
}

void
char_char_written(char *p, char i, char v)
{
        p[i] = v;
}

char *
char_char_values(char *p, char i)
{
        return (p + i) + ((p - i) - p);
}

signed char char_signed_char_g(void);

char
char_signed_char_index(char *p, signed char i)
{
        return p[i] + p[i + 1] + *(p + i) + *(p - i) + i[p];
}

char
char_signed_char_call(char *p)
{
        return p[char_signed_char_g()];
}

void
char_signed_char_written(char *p, signed char i, char v)
{
        p[i] = v;
}

char *
char_signed_char_values(char *p, signed char i)
{
        return (p + i) + ((p - i) - p);
}

unsigned char char_unsigned_char_g(void);

char
char_unsigned_char_index(char *p, unsigned char i)
{
        return p[i] + p[i + 1] + *(p + i) + *(p - i) + i[p];
}

char
char_unsigned_char_call(char *p)
{
        return p[char_unsigned_char_g()];
}

void
char_unsigned_char_written(char *p, unsigned char i, char v)
{
        p[i] = v;
}

char *
char_unsigned_char_values(char *p, unsigned char i)
{
        return (p + i) + ((p - i) - p);
}

short char_short_g(void);

char
char_short_index(char *p, short i)
{
        return p[i] + p[i + 1] + *(p + i) + *(p - i) + i[p];
}

char
char_short_call(char *p)
{
        return p[char_short_g()];
}

void
char_short_written(char *p, short i, char v)
{
        p[i] = v;
}

char *
char_short_values(char *p, short i)
{
        return (p + i) + ((p - i) - p);
}

unsigned short char_unsigned_short_g(void);

char
char_unsigned_short_index(char *p, unsigned short i)
{
        return p[i] + p[i + 1] + *(p + i) + *(p - i) + i[p];
}

char
char_unsigned_short_call(char *p)
{
        return p[char_unsigned_short_g()];
}

void
char_unsigned_short_written(char *p, unsigned short i, char v)
{
        p[i] = v;
}

char *
char_unsigned_short_values(char *p, unsigned short i)
{
        return (p + i) + ((p - i) - p);
}

int char_int_g(void);

char
char_int_index(char *p, int i)
{
        return p[i] + p[i + 1] + *(p + i) + *(p - i) + i[p];
}

char
char_int_call(char *p)
{
        return p[char_int_g()];
}

void
char_int_written(char *p, int i, char v)
{
        p[i] = v;
}

char *
char_int_values(char *p, int i)
{
        return (p + i) + ((p - i) - p);
}

unsigned char_unsigned_g(void);

char
char_unsigned_index(char *p, unsigned i)
{
        return p[i] + p[i + 1] + *(p + i) + *(p - i) + i[p];
}

char
char_unsigned_call(char *p)
{
        return p[char_unsigned_g()];
}

void
char_unsigned_written(char *p, unsigned i, char v)
{
        p[i] = v;
}

char *
char_unsigned_values(char *p, unsigned i)
{
        return (p + i) + ((p - i) - p);
}

long char_long_g(void);

char
char_long_index(char *p, long i)
{
        return p[i] + p[i + 1] + *(p + i) + *(p - i) + i[p];
}

char
char_long_call(char *p)
{
        return p[char_long_g()];
}

void
char_long_written(char *p, long i, char v)
{
        p[i] = v;
}

char *
char_long_values(char *p, long i)
{
        return (p + i) + ((p - i) - p);
}

unsigned long char_unsigned_long_g(void);

char
char_unsigned_long_index(char *p, unsigned long i)
{
        return p[i] + p[i + 1] + *(p + i) + *(p - i) + i[p];
}

char
char_unsigned_long_call(char *p)
{
        return p[char_unsigned_long_g()];
}

void
char_unsigned_long_written(char *p, unsigned long i, char v)
{
        p[i] = v;
}

char *
char_unsigned_long_values(char *p, unsigned long i)
{
        return (p + i) + ((p - i) - p);
}

char long_char_g(void);

long
long_char_index(long *p, char i)
{
        return p[i] + p[i + 1] + *(p + i) + *(p - i) + i[p];
}

long
long_char_call(long *p)
{
        return p[long_char_g()];
}

void
long_char_written(long *p, char i, long v)
{
        p[i] = v;
}

long *
long_char_values(long *p, char i)
{
        return (p + i) + ((p - i) - p);
}

signed char long_signed_char_g(void);

long
long_signed_char_index(long *p, signed char i)
{
        return p[i] + p[i + 1] + *(p + i) + *(p - i) + i[p];
}

long
long_signed_char_call(long *p)
{
        return p[long_signed_char_g()];
}

void
long_signed_char_written(long *p, signed char i, long v)
{
        p[i] = v;
}

long *
long_signed_char_values(long *p, signed char i)
{
        return (p + i) + ((p - i) - p);
}

unsigned char long_unsigned_char_g(void);

long
long_unsigned_char_index(long *p, unsigned char i)
{
        return p[i] + p[i + 1] + *(p + i) + *(p - i) + i[p];
}

long
long_unsigned_char_call(long *p)
{
        return p[long_unsigned_char_g()];
}

void
long_unsigned_char_written(long *p, unsigned char i, long v)
{
        p[i] = v;
}

long *
long_unsigned_char_values(long *p, unsigned char i)
{
        return (p + i) + ((p - i) - p);
}

short long_short_g(void);

long
long_short_index(long *p, short i)
{
        return p[i] + p[i + 1] + *(p + i) + *(p - i) + i[p];
}

long
long_short_call(long *p)
{
        return p[long_short_g()];
}

void
long_short_written(long *p, short i, long v)
{
        p[i] = v;
}

long *
long_short_values(long *p, short i)
{
        return (p + i) + ((p - i) - p);
}

unsigned short long_unsigned_short_g(void);

long
long_unsigned_short_index(long *p, unsigned short i)
{
        return p[i] + p[i + 1] + *(p + i) + *(p - i) + i[p];
}

long
long_unsigned_short_call(long *p)
{
        return p[long_unsigned_short_g()];
}

void
long_unsigned_short_written(long *p, unsigned short i, long v)
{
        p[i] = v;
}

long *
long_unsigned_short_values(long *p, unsigned short i)
{
        return (p + i) + ((p - i) - p);
}

int long_int_g(void);

long
long_int_index(long *p, int i)
{
        return p[i] + p[i + 1] + *(p + i) + *(p - i) + i[p];
}

long
long_int_call(long *p)
{
        return p[long_int_g()];
}

void
long_int_written(long *p, int i, long v)
{
        p[i] = v;
}

long *
long_int_values(long *p, int i)
{
        return (p + i) + ((p - i) - p);
}

unsigned long_unsigned_g(void);

long
long_unsigned_index(long *p, unsigned i)
{
        return p[i] + p[i + 1] + *(p + i) + *(p - i) + i[p];
}

long
long_unsigned_call(long *p)
{
        return p[long_unsigned_g()];
}

void
long_unsigned_written(long *p, unsigned i, long v)
{
        p[i] = v;
}

long *
long_unsigned_values(long *p, unsigned i)
{
        return (p + i) + ((p - i) - p);
}

long long_long_g(void);

long
long_long_index(long *p, long i)
{
        return p[i] + p[i + 1] + *(p + i) + *(p - i) + i[p];
}

long
long_long_call(long *p)
{
        return p[long_long_g()];
}

void
long_long_written(long *p, long i, long v)
{
        p[i] = v;
}

long *
long_long_values(long *p, long i)
{
        return (p + i) + ((p - i) - p);
}

unsigned long long_unsigned_long_g(void);

long
long_unsigned_long_index(long *p, unsigned long i)
{
        return p[i] + p[i + 1] + *(p + i) + *(p - i) + i[p];
}

long
long_unsigned_long_call(long *p)
{
        return p[long_unsigned_long_g()];
}

void
long_unsigned_long_written(long *p, unsigned long i, long v)
{
        p[i] = v;
}

long *
long_unsigned_long_values(long *p, unsigned long i)
{
        return (p + i) + ((p - i) - p);
}
