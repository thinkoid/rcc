/* *p through a pointer to each integer type as an operand: x86-64
   names an unsigned or a long object as the instruction's operand,
   addl (%rcx), %eax, addq (%rcx), %rax, imulq $5, (%rax), %rax, divl
   (%rcx), and loads and extends a narrow one first; aarch64 loads
   every one. */
enum e { A = -1, B = 7 };

long
char_arithmetic(char *p, char x)
{
        return (x + *p) + (x - *p) + (x & *p) + (x | *p) + (x ^ *p) + (x * *p) +
               (*p + x) + (*p - x) + (*p * 5) + (5 * *p) + (*p & 1);
}

long
char_divisions(char *p, char x)
{
        return (x / *p) + (*p / x) + (x % *p) + (*p % x) + (*p / 5);
}

long
char_shifts(char *p, int x)
{
        return (x << *p) + (*p << x) + (*p >> 3) + (x >> *p);
}

long
char_unary(char *p)
{
        return -*p + ~*p + !*p + +*p;
}

int
char_comparisons(char *p, char *q, char x)
{
        return (x < *p) + (*p < x) + (*p < 5) + (5 < *p) + (*p == 5) +
               (*p == 0) + (*p < *q) + (*p >= -1);
}

int
char_logical(char *p, char *q)
{
        return *p && *q || *p;
}

long
signed_char_arithmetic(signed char *p, signed char x)
{
        return (x + *p) + (x - *p) + (x & *p) + (x | *p) + (x ^ *p) + (x * *p) +
               (*p + x) + (*p - x) + (*p * 5) + (5 * *p) + (*p & 1);
}

long
signed_char_divisions(signed char *p, signed char x)
{
        return (x / *p) + (*p / x) + (x % *p) + (*p % x) + (*p / 5);
}

long
signed_char_shifts(signed char *p, int x)
{
        return (x << *p) + (*p << x) + (*p >> 3) + (x >> *p);
}

long
signed_char_unary(signed char *p)
{
        return -*p + ~*p + !*p + +*p;
}

int
signed_char_comparisons(signed char *p, signed char *q, signed char x)
{
        return (x < *p) + (*p < x) + (*p < 5) + (5 < *p) + (*p == 5) +
               (*p == 0) + (*p < *q) + (*p >= -1);
}

int
signed_char_logical(signed char *p, signed char *q)
{
        return *p && *q || *p;
}

long
unsigned_char_arithmetic(unsigned char *p, unsigned char x)
{
        return (x + *p) + (x - *p) + (x & *p) + (x | *p) + (x ^ *p) + (x * *p) +
               (*p + x) + (*p - x) + (*p * 5) + (5 * *p) + (*p & 1);
}

long
unsigned_char_divisions(unsigned char *p, unsigned char x)
{
        return (x / *p) + (*p / x) + (x % *p) + (*p % x) + (*p / 5);
}

long
unsigned_char_shifts(unsigned char *p, int x)
{
        return (x << *p) + (*p << x) + (*p >> 3) + (x >> *p);
}

long
unsigned_char_unary(unsigned char *p)
{
        return -*p + ~*p + !*p + +*p;
}

int
unsigned_char_comparisons(unsigned char *p, unsigned char *q, unsigned char x)
{
        return (x < *p) + (*p < x) + (*p < 5) + (5 < *p) + (*p == 5) +
               (*p == 0) + (*p < *q) + (*p >= -1);
}

int
unsigned_char_logical(unsigned char *p, unsigned char *q)
{
        return *p && *q || *p;
}

long
short_arithmetic(short *p, short x)
{
        return (x + *p) + (x - *p) + (x & *p) + (x | *p) + (x ^ *p) + (x * *p) +
               (*p + x) + (*p - x) + (*p * 5) + (5 * *p) + (*p & 1);
}

long
short_divisions(short *p, short x)
{
        return (x / *p) + (*p / x) + (x % *p) + (*p % x) + (*p / 5);
}

long
short_shifts(short *p, int x)
{
        return (x << *p) + (*p << x) + (*p >> 3) + (x >> *p);
}

long
short_unary(short *p)
{
        return -*p + ~*p + !*p + +*p;
}

int
short_comparisons(short *p, short *q, short x)
{
        return (x < *p) + (*p < x) + (*p < 5) + (5 < *p) + (*p == 5) +
               (*p == 0) + (*p < *q) + (*p >= -1);
}

int
short_logical(short *p, short *q)
{
        return *p && *q || *p;
}

long
unsigned_short_arithmetic(unsigned short *p, unsigned short x)
{
        return (x + *p) + (x - *p) + (x & *p) + (x | *p) + (x ^ *p) + (x * *p) +
               (*p + x) + (*p - x) + (*p * 5) + (5 * *p) + (*p & 1);
}

long
unsigned_short_divisions(unsigned short *p, unsigned short x)
{
        return (x / *p) + (*p / x) + (x % *p) + (*p % x) + (*p / 5);
}

long
unsigned_short_shifts(unsigned short *p, int x)
{
        return (x << *p) + (*p << x) + (*p >> 3) + (x >> *p);
}

long
unsigned_short_unary(unsigned short *p)
{
        return -*p + ~*p + !*p + +*p;
}

int
unsigned_short_comparisons(unsigned short *p,
                           unsigned short *q,
                           unsigned short x)
{
        return (x < *p) + (*p < x) + (*p < 5) + (5 < *p) + (*p == 5) +
               (*p == 0) + (*p < *q) + (*p >= -1);
}

int
unsigned_short_logical(unsigned short *p, unsigned short *q)
{
        return *p && *q || *p;
}

long
unsigned_arithmetic(unsigned *p, unsigned x)
{
        return (x + *p) + (x - *p) + (x & *p) + (x | *p) + (x ^ *p) + (x * *p) +
               (*p + x) + (*p - x) + (*p * 5) + (5 * *p) + (*p & 1);
}

long
unsigned_divisions(unsigned *p, unsigned x)
{
        return (x / *p) + (*p / x) + (x % *p) + (*p % x) + (*p / 5);
}

long
unsigned_shifts(unsigned *p, int x)
{
        return (x << *p) + (*p << x) + (*p >> 3) + (x >> *p);
}

long
unsigned_unary(unsigned *p)
{
        return -*p + ~*p + !*p + +*p;
}

int
unsigned_comparisons(unsigned *p, unsigned *q, unsigned x)
{
        return (x < *p) + (*p < x) + (*p < 5) + (5 < *p) + (*p == 5) +
               (*p == 0) + (*p < *q) + (*p >= -1);
}

int
unsigned_logical(unsigned *p, unsigned *q)
{
        return *p && *q || *p;
}

long
long_arithmetic(long *p, long x)
{
        return (x + *p) + (x - *p) + (x & *p) + (x | *p) + (x ^ *p) + (x * *p) +
               (*p + x) + (*p - x) + (*p * 5) + (5 * *p) + (*p & 1);
}

long
long_divisions(long *p, long x)
{
        return (x / *p) + (*p / x) + (x % *p) + (*p % x) + (*p / 5);
}

long
long_shifts(long *p, int x)
{
        return (x << *p) + (*p << x) + (*p >> 3) + (x >> *p);
}

long
long_unary(long *p)
{
        return -*p + ~*p + !*p + +*p;
}

int
long_comparisons(long *p, long *q, long x)
{
        return (x < *p) + (*p < x) + (*p < 5) + (5 < *p) + (*p == 5) +
               (*p == 0) + (*p < *q) + (*p >= -1);
}

int
long_logical(long *p, long *q)
{
        return *p && *q || *p;
}

long
unsigned_long_arithmetic(unsigned long *p, unsigned long x)
{
        return (x + *p) + (x - *p) + (x & *p) + (x | *p) + (x ^ *p) + (x * *p) +
               (*p + x) + (*p - x) + (*p * 5) + (5 * *p) + (*p & 1);
}

long
unsigned_long_divisions(unsigned long *p, unsigned long x)
{
        return (x / *p) + (*p / x) + (x % *p) + (*p % x) + (*p / 5);
}

long
unsigned_long_shifts(unsigned long *p, int x)
{
        return (x << *p) + (*p << x) + (*p >> 3) + (x >> *p);
}

long
unsigned_long_unary(unsigned long *p)
{
        return -*p + ~*p + !*p + +*p;
}

int
unsigned_long_comparisons(unsigned long *p, unsigned long *q, unsigned long x)
{
        return (x < *p) + (*p < x) + (*p < 5) + (5 < *p) + (*p == 5) +
               (*p == 0) + (*p < *q) + (*p >= -1);
}

int
unsigned_long_logical(unsigned long *p, unsigned long *q)
{
        return *p && *q || *p;
}

long
long_long_arithmetic(long long *p, long long x)
{
        return (x + *p) + (x - *p) + (x & *p) + (x | *p) + (x ^ *p) + (x * *p) +
               (*p + x) + (*p - x) + (*p * 5) + (5 * *p) + (*p & 1);
}

long
long_long_divisions(long long *p, long long x)
{
        return (x / *p) + (*p / x) + (x % *p) + (*p % x) + (*p / 5);
}

long
long_long_shifts(long long *p, int x)
{
        return (x << *p) + (*p << x) + (*p >> 3) + (x >> *p);
}

long
long_long_unary(long long *p)
{
        return -*p + ~*p + !*p + +*p;
}

int
long_long_comparisons(long long *p, long long *q, long long x)
{
        return (x < *p) + (*p < x) + (*p < 5) + (5 < *p) + (*p == 5) +
               (*p == 0) + (*p < *q) + (*p >= -1);
}

int
long_long_logical(long long *p, long long *q)
{
        return *p && *q || *p;
}

long
unsigned_long_long_arithmetic(unsigned long long *p, unsigned long long x)
{
        return (x + *p) + (x - *p) + (x & *p) + (x | *p) + (x ^ *p) + (x * *p) +
               (*p + x) + (*p - x) + (*p * 5) + (5 * *p) + (*p & 1);
}

long
unsigned_long_long_divisions(unsigned long long *p, unsigned long long x)
{
        return (x / *p) + (*p / x) + (x % *p) + (*p % x) + (*p / 5);
}

long
unsigned_long_long_shifts(unsigned long long *p, int x)
{
        return (x << *p) + (*p << x) + (*p >> 3) + (x >> *p);
}

long
unsigned_long_long_unary(unsigned long long *p)
{
        return -*p + ~*p + !*p + +*p;
}

int
unsigned_long_long_comparisons(unsigned long long *p,
                               unsigned long long *q,
                               unsigned long long x)
{
        return (x < *p) + (*p < x) + (*p < 5) + (5 < *p) + (*p == 5) +
               (*p == 0) + (*p < *q) + (*p >= -1);
}

int
unsigned_long_long_logical(unsigned long long *p, unsigned long long *q)
{
        return *p && *q || *p;
}

long
enum_arithmetic(enum e *p, enum e x)
{
        return (x + *p) + (x - *p) + (x & *p) + (x | *p) + (x ^ *p) + (x * *p) +
               (*p + x) + (*p - x) + (*p * 5) + (5 * *p) + (*p & 1);
}

long
enum_divisions(enum e *p, enum e x)
{
        return (x / *p) + (*p / x) + (x % *p) + (*p % x) + (*p / 5);
}

long
enum_shifts(enum e *p, int x)
{
        return (x << *p) + (*p << x) + (*p >> 3) + (x >> *p);
}

long
enum_unary(enum e *p)
{
        return -*p + ~*p + !*p + +*p;
}

int
enum_comparisons(enum e *p, enum e *q, enum e x)
{
        return (x < *p) + (*p < x) + (*p < 5) + (5 < *p) + (*p == 5) +
               (*p == 0) + (*p < *q) + (*p >= -1);
}

int
enum_logical(enum e *p, enum e *q)
{
        return *p && *q || *p;
}
