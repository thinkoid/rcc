/* Pointers to each integer type as values: & of a local, declared,
   initialized, assigned, passed, returned, compared, tested, cast to
   another pointer type, which writes nothing, and to an integer, and
   sizeof of the pointer, of the pointee and of *p promoted; pointers to
   those pointers. */
enum e { A = -1, B = 7 };

char *char_g(char *p, char **pp);

char *
char_values(char x, char *q)
{
        char y = 3;
        char *p = &x;
        char **pp = &p;
        p = &y;
        *pp = q;
        return char_g(p, pp);
}

int
char_compared(char *p, char *q)
{
        char x = 1;
        return (p < q) + (p == q) + (p != &x) + (&x >= q) + (p == 0) + !q +
               (p && q);
}

long
char_casts(char *p)
{
        int *i = (int *) p;
        char *c = (char *) p;
        char **pp = (char **) c;
        return (long) p + (unsigned long) i + (long) (char *) pp;
}

unsigned long
char_sizes(char *p)
{
        return sizeof p + sizeof *p * 10 + sizeof +*p * 100 +
               sizeof(char *) * 1000 + sizeof(char) * 10000;
}

char *
char_tested(char *p, char *q)
{
        if (p)
                return p;
        while (!q)
                q = p;
        return q;
}

signed char *signed_char_g(signed char *p, signed char **pp);

signed char *
signed_char_values(signed char x, signed char *q)
{
        signed char y = 3;
        signed char *p = &x;
        signed char **pp = &p;
        p = &y;
        *pp = q;
        return signed_char_g(p, pp);
}

int
signed_char_compared(signed char *p, signed char *q)
{
        signed char x = 1;
        return (p < q) + (p == q) + (p != &x) + (&x >= q) + (p == 0) + !q +
               (p && q);
}

long
signed_char_casts(signed char *p)
{
        int *i = (int *) p;
        char *c = (char *) p;
        signed char **pp = (signed char **) c;
        return (long) p + (unsigned long) i + (long) (signed char *) pp;
}

unsigned long
signed_char_sizes(signed char *p)
{
        return sizeof p + sizeof *p * 10 + sizeof +*p * 100 +
               sizeof(signed char *) * 1000 + sizeof(signed char) * 10000;
}

signed char *
signed_char_tested(signed char *p, signed char *q)
{
        if (p)
                return p;
        while (!q)
                q = p;
        return q;
}

unsigned char *unsigned_char_g(unsigned char *p, unsigned char **pp);

unsigned char *
unsigned_char_values(unsigned char x, unsigned char *q)
{
        unsigned char y = 3;
        unsigned char *p = &x;
        unsigned char **pp = &p;
        p = &y;
        *pp = q;
        return unsigned_char_g(p, pp);
}

int
unsigned_char_compared(unsigned char *p, unsigned char *q)
{
        unsigned char x = 1;
        return (p < q) + (p == q) + (p != &x) + (&x >= q) + (p == 0) + !q +
               (p && q);
}

long
unsigned_char_casts(unsigned char *p)
{
        int *i = (int *) p;
        char *c = (char *) p;
        unsigned char **pp = (unsigned char **) c;
        return (long) p + (unsigned long) i + (long) (unsigned char *) pp;
}

unsigned long
unsigned_char_sizes(unsigned char *p)
{
        return sizeof p + sizeof *p * 10 + sizeof +*p * 100 +
               sizeof(unsigned char *) * 1000 + sizeof(unsigned char) * 10000;
}

unsigned char *
unsigned_char_tested(unsigned char *p, unsigned char *q)
{
        if (p)
                return p;
        while (!q)
                q = p;
        return q;
}

short *short_g(short *p, short **pp);

short *
short_values(short x, short *q)
{
        short y = 3;
        short *p = &x;
        short **pp = &p;
        p = &y;
        *pp = q;
        return short_g(p, pp);
}

int
short_compared(short *p, short *q)
{
        short x = 1;
        return (p < q) + (p == q) + (p != &x) + (&x >= q) + (p == 0) + !q +
               (p && q);
}

long
short_casts(short *p)
{
        int *i = (int *) p;
        char *c = (char *) p;
        short **pp = (short **) c;
        return (long) p + (unsigned long) i + (long) (short *) pp;
}

unsigned long
short_sizes(short *p)
{
        return sizeof p + sizeof *p * 10 + sizeof +*p * 100 +
               sizeof(short *) * 1000 + sizeof(short) * 10000;
}

short *
short_tested(short *p, short *q)
{
        if (p)
                return p;
        while (!q)
                q = p;
        return q;
}

unsigned short *unsigned_short_g(unsigned short *p, unsigned short **pp);

unsigned short *
unsigned_short_values(unsigned short x, unsigned short *q)
{
        unsigned short y = 3;
        unsigned short *p = &x;
        unsigned short **pp = &p;
        p = &y;
        *pp = q;
        return unsigned_short_g(p, pp);
}

int
unsigned_short_compared(unsigned short *p, unsigned short *q)
{
        unsigned short x = 1;
        return (p < q) + (p == q) + (p != &x) + (&x >= q) + (p == 0) + !q +
               (p && q);
}

long
unsigned_short_casts(unsigned short *p)
{
        int *i = (int *) p;
        char *c = (char *) p;
        unsigned short **pp = (unsigned short **) c;
        return (long) p + (unsigned long) i + (long) (unsigned short *) pp;
}

unsigned long
unsigned_short_sizes(unsigned short *p)
{
        return sizeof p + sizeof *p * 10 + sizeof +*p * 100 +
               sizeof(unsigned short *) * 1000 + sizeof(unsigned short) * 10000;
}

unsigned short *
unsigned_short_tested(unsigned short *p, unsigned short *q)
{
        if (p)
                return p;
        while (!q)
                q = p;
        return q;
}

unsigned *unsigned_g(unsigned *p, unsigned **pp);

unsigned *
unsigned_values(unsigned x, unsigned *q)
{
        unsigned y = 3;
        unsigned *p = &x;
        unsigned **pp = &p;
        p = &y;
        *pp = q;
        return unsigned_g(p, pp);
}

int
unsigned_compared(unsigned *p, unsigned *q)
{
        unsigned x = 1;
        return (p < q) + (p == q) + (p != &x) + (&x >= q) + (p == 0) + !q +
               (p && q);
}

long
unsigned_casts(unsigned *p)
{
        int *i = (int *) p;
        char *c = (char *) p;
        unsigned **pp = (unsigned **) c;
        return (long) p + (unsigned long) i + (long) (unsigned *) pp;
}

unsigned long
unsigned_sizes(unsigned *p)
{
        return sizeof p + sizeof *p * 10 + sizeof +*p * 100 +
               sizeof(unsigned *) * 1000 + sizeof(unsigned) * 10000;
}

unsigned *
unsigned_tested(unsigned *p, unsigned *q)
{
        if (p)
                return p;
        while (!q)
                q = p;
        return q;
}

long *long_g(long *p, long **pp);

long *
long_values(long x, long *q)
{
        long y = 3;
        long *p = &x;
        long **pp = &p;
        p = &y;
        *pp = q;
        return long_g(p, pp);
}

int
long_compared(long *p, long *q)
{
        long x = 1;
        return (p < q) + (p == q) + (p != &x) + (&x >= q) + (p == 0) + !q +
               (p && q);
}

long
long_casts(long *p)
{
        int *i = (int *) p;
        char *c = (char *) p;
        long **pp = (long **) c;
        return (long) p + (unsigned long) i + (long) (long *) pp;
}

unsigned long
long_sizes(long *p)
{
        return sizeof p + sizeof *p * 10 + sizeof +*p * 100 +
               sizeof(long *) * 1000 + sizeof(long) * 10000;
}

long *
long_tested(long *p, long *q)
{
        if (p)
                return p;
        while (!q)
                q = p;
        return q;
}

unsigned long *unsigned_long_g(unsigned long *p, unsigned long **pp);

unsigned long *
unsigned_long_values(unsigned long x, unsigned long *q)
{
        unsigned long y = 3;
        unsigned long *p = &x;
        unsigned long **pp = &p;
        p = &y;
        *pp = q;
        return unsigned_long_g(p, pp);
}

int
unsigned_long_compared(unsigned long *p, unsigned long *q)
{
        unsigned long x = 1;
        return (p < q) + (p == q) + (p != &x) + (&x >= q) + (p == 0) + !q +
               (p && q);
}

long
unsigned_long_casts(unsigned long *p)
{
        int *i = (int *) p;
        char *c = (char *) p;
        unsigned long **pp = (unsigned long **) c;
        return (long) p + (unsigned long) i + (long) (unsigned long *) pp;
}

unsigned long
unsigned_long_sizes(unsigned long *p)
{
        return sizeof p + sizeof *p * 10 + sizeof +*p * 100 +
               sizeof(unsigned long *) * 1000 + sizeof(unsigned long) * 10000;
}

unsigned long *
unsigned_long_tested(unsigned long *p, unsigned long *q)
{
        if (p)
                return p;
        while (!q)
                q = p;
        return q;
}

long long *long_long_g(long long *p, long long **pp);

long long *
long_long_values(long long x, long long *q)
{
        long long y = 3;
        long long *p = &x;
        long long **pp = &p;
        p = &y;
        *pp = q;
        return long_long_g(p, pp);
}

int
long_long_compared(long long *p, long long *q)
{
        long long x = 1;
        return (p < q) + (p == q) + (p != &x) + (&x >= q) + (p == 0) + !q +
               (p && q);
}

long
long_long_casts(long long *p)
{
        int *i = (int *) p;
        char *c = (char *) p;
        long long **pp = (long long **) c;
        return (long) p + (unsigned long) i + (long) (long long *) pp;
}

unsigned long
long_long_sizes(long long *p)
{
        return sizeof p + sizeof *p * 10 + sizeof +*p * 100 +
               sizeof(long long *) * 1000 + sizeof(long long) * 10000;
}

long long *
long_long_tested(long long *p, long long *q)
{
        if (p)
                return p;
        while (!q)
                q = p;
        return q;
}

unsigned long long *unsigned_long_long_g(unsigned long long *p,
                                         unsigned long long **pp);

unsigned long long *
unsigned_long_long_values(unsigned long long x, unsigned long long *q)
{
        unsigned long long y = 3;
        unsigned long long *p = &x;
        unsigned long long **pp = &p;
        p = &y;
        *pp = q;
        return unsigned_long_long_g(p, pp);
}

int
unsigned_long_long_compared(unsigned long long *p, unsigned long long *q)
{
        unsigned long long x = 1;
        return (p < q) + (p == q) + (p != &x) + (&x >= q) + (p == 0) + !q +
               (p && q);
}

long
unsigned_long_long_casts(unsigned long long *p)
{
        int *i = (int *) p;
        char *c = (char *) p;
        unsigned long long **pp = (unsigned long long **) c;
        return (long) p + (unsigned long) i + (long) (unsigned long long *) pp;
}

unsigned long
unsigned_long_long_sizes(unsigned long long *p)
{
        return sizeof p + sizeof *p * 10 + sizeof +*p * 100 +
               sizeof(unsigned long long *) * 1000 +
               sizeof(unsigned long long) * 10000;
}

unsigned long long *
unsigned_long_long_tested(unsigned long long *p, unsigned long long *q)
{
        if (p)
                return p;
        while (!q)
                q = p;
        return q;
}

enum e *enum_g(enum e *p, enum e **pp);

enum e *
enum_values(enum e x, enum e *q)
{
        enum e y = 3;
        enum e *p = &x;
        enum e **pp = &p;
        p = &y;
        *pp = q;
        return enum_g(p, pp);
}

int
enum_compared(enum e *p, enum e *q)
{
        enum e x = 1;
        return (p < q) + (p == q) + (p != &x) + (&x >= q) + (p == 0) + !q +
               (p && q);
}

long
enum_casts(enum e *p)
{
        int *i = (int *) p;
        char *c = (char *) p;
        enum e **pp = (enum e **) c;
        return (long) p + (unsigned long) i + (long) (enum e *) pp;
}

unsigned long
enum_sizes(enum e *p)
{
        return sizeof p + sizeof *p * 10 + sizeof +*p * 100 +
               sizeof(enum e *) * 1000 + sizeof(enum e) * 10000;
}

enum e *
enum_tested(enum e *p, enum e *q)
{
        if (p)
                return p;
        while (!q)
                q = p;
        return q;
}
