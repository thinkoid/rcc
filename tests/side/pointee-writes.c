/* *p written through a pointer to each integer type: the narrow
   store, movb %cl, (%rax), strh w1, [x0]; a constant in one movb $7,
   (%rax) or mov w1, #7 and the store, a zero strb wzr, a -1 at the
   pointee's value, mov w1, #255 for an unsigned char; and the value of
   the assignment, the stored value converted to the pointee's type. */
enum e { A = -1, B = 7 };

void
char_value(char *p, char v)
{
        *p = v;
}

void
char_constants(char *p, char *q, char *r)
{
        *p = 7;
        *q = 0;
        *r = -1;
}

char
char_returned(char *p, char v)
{
        return *p = v;
}

int
char_widened(char *p, int x)
{
        return (*p = x) + 1;
}

void
char_through(char **pp, char *q)
{
        **pp = *q;
        *pp = q;
}

void
signed_char_value(signed char *p, signed char v)
{
        *p = v;
}

void
signed_char_constants(signed char *p, signed char *q, signed char *r)
{
        *p = 7;
        *q = 0;
        *r = -1;
}

signed char
signed_char_returned(signed char *p, signed char v)
{
        return *p = v;
}

int
signed_char_widened(signed char *p, int x)
{
        return (*p = x) + 1;
}

void
signed_char_through(signed char **pp, signed char *q)
{
        **pp = *q;
        *pp = q;
}

void
unsigned_char_value(unsigned char *p, unsigned char v)
{
        *p = v;
}

void
unsigned_char_constants(unsigned char *p, unsigned char *q, unsigned char *r)
{
        *p = 7;
        *q = 0;
        *r = -1;
}

unsigned char
unsigned_char_returned(unsigned char *p, unsigned char v)
{
        return *p = v;
}

int
unsigned_char_widened(unsigned char *p, int x)
{
        return (*p = x) + 1;
}

void
unsigned_char_through(unsigned char **pp, unsigned char *q)
{
        **pp = *q;
        *pp = q;
}

void
short_value(short *p, short v)
{
        *p = v;
}

void
short_constants(short *p, short *q, short *r)
{
        *p = 7;
        *q = 0;
        *r = -1;
}

short
short_returned(short *p, short v)
{
        return *p = v;
}

int
short_widened(short *p, int x)
{
        return (*p = x) + 1;
}

void
short_through(short **pp, short *q)
{
        **pp = *q;
        *pp = q;
}

void
unsigned_short_value(unsigned short *p, unsigned short v)
{
        *p = v;
}

void
unsigned_short_constants(unsigned short *p,
                         unsigned short *q,
                         unsigned short *r)
{
        *p = 7;
        *q = 0;
        *r = -1;
}

unsigned short
unsigned_short_returned(unsigned short *p, unsigned short v)
{
        return *p = v;
}

int
unsigned_short_widened(unsigned short *p, int x)
{
        return (*p = x) + 1;
}

void
unsigned_short_through(unsigned short **pp, unsigned short *q)
{
        **pp = *q;
        *pp = q;
}

void
unsigned_value(unsigned *p, unsigned v)
{
        *p = v;
}

void
unsigned_constants(unsigned *p, unsigned *q, unsigned *r)
{
        *p = 7;
        *q = 0;
        *r = -1;
}

unsigned
unsigned_returned(unsigned *p, unsigned v)
{
        return *p = v;
}

int
unsigned_widened(unsigned *p, int x)
{
        return (*p = x) + 1;
}

void
unsigned_through(unsigned **pp, unsigned *q)
{
        **pp = *q;
        *pp = q;
}

void
long_value(long *p, long v)
{
        *p = v;
}

void
long_constants(long *p, long *q, long *r)
{
        *p = 7;
        *q = 0;
        *r = -1;
}

long
long_returned(long *p, long v)
{
        return *p = v;
}

int
long_widened(long *p, int x)
{
        return (*p = x) + 1;
}

void
long_through(long **pp, long *q)
{
        **pp = *q;
        *pp = q;
}

void
unsigned_long_value(unsigned long *p, unsigned long v)
{
        *p = v;
}

void
unsigned_long_constants(unsigned long *p, unsigned long *q, unsigned long *r)
{
        *p = 7;
        *q = 0;
        *r = -1;
}

unsigned long
unsigned_long_returned(unsigned long *p, unsigned long v)
{
        return *p = v;
}

int
unsigned_long_widened(unsigned long *p, int x)
{
        return (*p = x) + 1;
}

void
unsigned_long_through(unsigned long **pp, unsigned long *q)
{
        **pp = *q;
        *pp = q;
}

void
long_long_value(long long *p, long long v)
{
        *p = v;
}

void
long_long_constants(long long *p, long long *q, long long *r)
{
        *p = 7;
        *q = 0;
        *r = -1;
}

long long
long_long_returned(long long *p, long long v)
{
        return *p = v;
}

int
long_long_widened(long long *p, int x)
{
        return (*p = x) + 1;
}

void
long_long_through(long long **pp, long long *q)
{
        **pp = *q;
        *pp = q;
}

void
unsigned_long_long_value(unsigned long long *p, unsigned long long v)
{
        *p = v;
}

void
unsigned_long_long_constants(unsigned long long *p,
                             unsigned long long *q,
                             unsigned long long *r)
{
        *p = 7;
        *q = 0;
        *r = -1;
}

unsigned long long
unsigned_long_long_returned(unsigned long long *p, unsigned long long v)
{
        return *p = v;
}

int
unsigned_long_long_widened(unsigned long long *p, int x)
{
        return (*p = x) + 1;
}

void
unsigned_long_long_through(unsigned long long **pp, unsigned long long *q)
{
        **pp = *q;
        *pp = q;
}

void
enum_value(enum e *p, enum e v)
{
        *p = v;
}

void
enum_constants(enum e *p, enum e *q, enum e *r)
{
        *p = 7;
        *q = 0;
        *r = -1;
}

enum e
enum_returned(enum e *p, enum e v)
{
        return *p = v;
}

int
enum_widened(enum e *p, int x)
{
        return (*p = x) + 1;
}

void
enum_through(enum e **pp, enum e *q)
{
        **pp = *q;
        *pp = q;
}
