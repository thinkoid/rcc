/* Pointer arithmetic at the strides of each integer type, 1, 2, 4 and
   8: the constant times the stride, addq $3, %rax on a char *, add x0,
   x0, #6 on a short *; a variable index at a scale of 2, 4 or 8 the
   leaq (%rax,%rcx,2), and at a stride of 1 the addq of the index; the
   index written first; p - i, whose product is multiplied but at a
   stride of 1; and p - l on a long in memory, subq -16(%rbp), %rax at a
   stride of 1, imulq $8, -16(%rbp), %rcx at 8. */
char *
char_constants(char *p)
{
        char *q = p + 3;
        q = 3 + q;
        q = q - 3;
        return q + 1000000000;
}

char *
char_index(char *p, int i)
{
        return p + i;
}

char *
char_index_first(char *p, int i)
{
        return i + p;
}

char *
char_less(char *p, int i)
{
        return p - i;
}

char *
char_less_long(char *p, long l)
{
        return p - l;
}

char *
char_plus_long(char *p, long l)
{
        return (p + l) + (l + p - l);
}

long
char_difference(char *p, char *q)
{
        return (p - q) + (q - p) / 2;
}

int
char_difference_compared(char *p, char *q)
{
        if (p - q)
                return p - q < 3;
        return 0;
}

char *
char_address(char *p, int i)
{
        return &p[i] + (&p[3] - p);
}

signed char *
signed_char_constants(signed char *p)
{
        signed char *q = p + 3;
        q = 3 + q;
        q = q - 3;
        return q + 1000000000;
}

signed char *
signed_char_index(signed char *p, int i)
{
        return p + i;
}

signed char *
signed_char_index_first(signed char *p, int i)
{
        return i + p;
}

signed char *
signed_char_less(signed char *p, int i)
{
        return p - i;
}

signed char *
signed_char_less_long(signed char *p, long l)
{
        return p - l;
}

signed char *
signed_char_plus_long(signed char *p, long l)
{
        return (p + l) + (l + p - l);
}

long
signed_char_difference(signed char *p, signed char *q)
{
        return (p - q) + (q - p) / 2;
}

int
signed_char_difference_compared(signed char *p, signed char *q)
{
        if (p - q)
                return p - q < 3;
        return 0;
}

signed char *
signed_char_address(signed char *p, int i)
{
        return &p[i] + (&p[3] - p);
}

unsigned char *
unsigned_char_constants(unsigned char *p)
{
        unsigned char *q = p + 3;
        q = 3 + q;
        q = q - 3;
        return q + 1000000000;
}

unsigned char *
unsigned_char_index(unsigned char *p, int i)
{
        return p + i;
}

unsigned char *
unsigned_char_index_first(unsigned char *p, int i)
{
        return i + p;
}

unsigned char *
unsigned_char_less(unsigned char *p, int i)
{
        return p - i;
}

unsigned char *
unsigned_char_less_long(unsigned char *p, long l)
{
        return p - l;
}

unsigned char *
unsigned_char_plus_long(unsigned char *p, long l)
{
        return (p + l) + (l + p - l);
}

long
unsigned_char_difference(unsigned char *p, unsigned char *q)
{
        return (p - q) + (q - p) / 2;
}

int
unsigned_char_difference_compared(unsigned char *p, unsigned char *q)
{
        if (p - q)
                return p - q < 3;
        return 0;
}

unsigned char *
unsigned_char_address(unsigned char *p, int i)
{
        return &p[i] + (&p[3] - p);
}

short *
short_constants(short *p)
{
        short *q = p + 3;
        q = 3 + q;
        q = q - 3;
        return q + 1000000000;
}

short *
short_index(short *p, int i)
{
        return p + i;
}

short *
short_index_first(short *p, int i)
{
        return i + p;
}

short *
short_less(short *p, int i)
{
        return p - i;
}

short *
short_less_long(short *p, long l)
{
        return p - l;
}

short *
short_plus_long(short *p, long l)
{
        return (p + l) + (l + p - l);
}

long
short_difference(short *p, short *q)
{
        return (p - q) + (q - p) / 2;
}

int
short_difference_compared(short *p, short *q)
{
        if (p - q)
                return p - q < 3;
        return 0;
}

short *
short_address(short *p, int i)
{
        return &p[i] + (&p[3] - p);
}

unsigned short *
unsigned_short_constants(unsigned short *p)
{
        unsigned short *q = p + 3;
        q = 3 + q;
        q = q - 3;
        return q + 1000000000;
}

unsigned short *
unsigned_short_index(unsigned short *p, int i)
{
        return p + i;
}

unsigned short *
unsigned_short_index_first(unsigned short *p, int i)
{
        return i + p;
}

unsigned short *
unsigned_short_less(unsigned short *p, int i)
{
        return p - i;
}

unsigned short *
unsigned_short_less_long(unsigned short *p, long l)
{
        return p - l;
}

unsigned short *
unsigned_short_plus_long(unsigned short *p, long l)
{
        return (p + l) + (l + p - l);
}

long
unsigned_short_difference(unsigned short *p, unsigned short *q)
{
        return (p - q) + (q - p) / 2;
}

int
unsigned_short_difference_compared(unsigned short *p, unsigned short *q)
{
        if (p - q)
                return p - q < 3;
        return 0;
}

unsigned short *
unsigned_short_address(unsigned short *p, int i)
{
        return &p[i] + (&p[3] - p);
}

unsigned *
unsigned_constants(unsigned *p)
{
        unsigned *q = p + 3;
        q = 3 + q;
        q = q - 3;
        return q + 1000000000;
}

unsigned *
unsigned_index(unsigned *p, int i)
{
        return p + i;
}

unsigned *
unsigned_index_first(unsigned *p, int i)
{
        return i + p;
}

unsigned *
unsigned_less(unsigned *p, int i)
{
        return p - i;
}

unsigned *
unsigned_less_long(unsigned *p, long l)
{
        return p - l;
}

unsigned *
unsigned_plus_long(unsigned *p, long l)
{
        return (p + l) + (l + p - l);
}

long
unsigned_difference(unsigned *p, unsigned *q)
{
        return (p - q) + (q - p) / 2;
}

int
unsigned_difference_compared(unsigned *p, unsigned *q)
{
        if (p - q)
                return p - q < 3;
        return 0;
}

unsigned *
unsigned_address(unsigned *p, int i)
{
        return &p[i] + (&p[3] - p);
}

long *
long_constants(long *p)
{
        long *q = p + 3;
        q = 3 + q;
        q = q - 3;
        return q + 1000000000;
}

long *
long_index(long *p, int i)
{
        return p + i;
}

long *
long_index_first(long *p, int i)
{
        return i + p;
}

long *
long_less(long *p, int i)
{
        return p - i;
}

long *
long_less_long(long *p, long l)
{
        return p - l;
}

long *
long_plus_long(long *p, long l)
{
        return (p + l) + (l + p - l);
}

long
long_difference(long *p, long *q)
{
        return (p - q) + (q - p) / 2;
}

int
long_difference_compared(long *p, long *q)
{
        if (p - q)
                return p - q < 3;
        return 0;
}

long *
long_address(long *p, int i)
{
        return &p[i] + (&p[3] - p);
}

unsigned long *
unsigned_long_constants(unsigned long *p)
{
        unsigned long *q = p + 3;
        q = 3 + q;
        q = q - 3;
        return q + 1000000000;
}

unsigned long *
unsigned_long_index(unsigned long *p, int i)
{
        return p + i;
}

unsigned long *
unsigned_long_index_first(unsigned long *p, int i)
{
        return i + p;
}

unsigned long *
unsigned_long_less(unsigned long *p, int i)
{
        return p - i;
}

unsigned long *
unsigned_long_less_long(unsigned long *p, long l)
{
        return p - l;
}

unsigned long *
unsigned_long_plus_long(unsigned long *p, long l)
{
        return (p + l) + (l + p - l);
}

long
unsigned_long_difference(unsigned long *p, unsigned long *q)
{
        return (p - q) + (q - p) / 2;
}

int
unsigned_long_difference_compared(unsigned long *p, unsigned long *q)
{
        if (p - q)
                return p - q < 3;
        return 0;
}

unsigned long *
unsigned_long_address(unsigned long *p, int i)
{
        return &p[i] + (&p[3] - p);
}

long long *
long_long_constants(long long *p)
{
        long long *q = p + 3;
        q = 3 + q;
        q = q - 3;
        return q + 1000000000;
}

long long *
long_long_index(long long *p, int i)
{
        return p + i;
}

long long *
long_long_index_first(long long *p, int i)
{
        return i + p;
}

long long *
long_long_less(long long *p, int i)
{
        return p - i;
}

long long *
long_long_less_long(long long *p, long l)
{
        return p - l;
}

long long *
long_long_plus_long(long long *p, long l)
{
        return (p + l) + (l + p - l);
}

long
long_long_difference(long long *p, long long *q)
{
        return (p - q) + (q - p) / 2;
}

int
long_long_difference_compared(long long *p, long long *q)
{
        if (p - q)
                return p - q < 3;
        return 0;
}

long long *
long_long_address(long long *p, int i)
{
        return &p[i] + (&p[3] - p);
}

unsigned long long *
unsigned_long_long_constants(unsigned long long *p)
{
        unsigned long long *q = p + 3;
        q = 3 + q;
        q = q - 3;
        return q + 1000000000;
}

unsigned long long *
unsigned_long_long_index(unsigned long long *p, int i)
{
        return p + i;
}

unsigned long long *
unsigned_long_long_index_first(unsigned long long *p, int i)
{
        return i + p;
}

unsigned long long *
unsigned_long_long_less(unsigned long long *p, int i)
{
        return p - i;
}

unsigned long long *
unsigned_long_long_less_long(unsigned long long *p, long l)
{
        return p - l;
}

unsigned long long *
unsigned_long_long_plus_long(unsigned long long *p, long l)
{
        return (p + l) + (l + p - l);
}

long
unsigned_long_long_difference(unsigned long long *p, unsigned long long *q)
{
        return (p - q) + (q - p) / 2;
}

int
unsigned_long_long_difference_compared(unsigned long long *p,
                                       unsigned long long *q)
{
        if (p - q)
                return p - q < 3;
        return 0;
}

unsigned long long *
unsigned_long_long_address(unsigned long long *p, int i)
{
        return &p[i] + (&p[3] - p);
}
