/* *p read through a pointer to each integer type: the load at the
   pointee's width and by its sign, movsbl (%rax), %eax, movzwl and movq,
   ldrsb w0, [x0], ldrh and ldr x0; widened to 64 bits, the extending
   load of a signed pointee, movsbq (%rax), %rax, ldrsh x0, [x0]; and of
   an enumerated type, its integer type's. */
enum e { A = -1, B = 7 };

char
char_read(char *p)
{
        return *p;
}

long
char_long(char *p)
{
        return *p;
}

unsigned long
char_unsigned_long(char *p)
{
        return *p;
}

int
char_int(char **pp)
{
        return **pp + 1;
}

signed char
signed_char_read(signed char *p)
{
        return *p;
}

long
signed_char_long(signed char *p)
{
        return *p;
}

unsigned long
signed_char_unsigned_long(signed char *p)
{
        return *p;
}

int
signed_char_int(signed char **pp)
{
        return **pp + 1;
}

unsigned char
unsigned_char_read(unsigned char *p)
{
        return *p;
}

long
unsigned_char_long(unsigned char *p)
{
        return *p;
}

unsigned long
unsigned_char_unsigned_long(unsigned char *p)
{
        return *p;
}

int
unsigned_char_int(unsigned char **pp)
{
        return **pp + 1;
}

short
short_read(short *p)
{
        return *p;
}

long
short_long(short *p)
{
        return *p;
}

unsigned long
short_unsigned_long(short *p)
{
        return *p;
}

int
short_int(short **pp)
{
        return **pp + 1;
}

unsigned short
unsigned_short_read(unsigned short *p)
{
        return *p;
}

long
unsigned_short_long(unsigned short *p)
{
        return *p;
}

unsigned long
unsigned_short_unsigned_long(unsigned short *p)
{
        return *p;
}

int
unsigned_short_int(unsigned short **pp)
{
        return **pp + 1;
}

unsigned
unsigned_read(unsigned *p)
{
        return *p;
}

long
unsigned_long(unsigned *p)
{
        return *p;
}

unsigned long
unsigned_unsigned_long(unsigned *p)
{
        return *p;
}

int
unsigned_int(unsigned **pp)
{
        return **pp + 1;
}

long
long_read(long *p)
{
        return *p;
}

long
long_long(long *p)
{
        return *p;
}

unsigned long
long_unsigned_long(long *p)
{
        return *p;
}

int
long_int(long **pp)
{
        return **pp + 1;
}

unsigned long
unsigned_long_read(unsigned long *p)
{
        return *p;
}

long
unsigned_long_long(unsigned long *p)
{
        return *p;
}

unsigned long
unsigned_long_unsigned_long(unsigned long *p)
{
        return *p;
}

int
unsigned_long_int(unsigned long **pp)
{
        return **pp + 1;
}

long long
long_long_read(long long *p)
{
        return *p;
}

long
long_long_long(long long *p)
{
        return *p;
}

unsigned long
long_long_unsigned_long(long long *p)
{
        return *p;
}

int
long_long_int(long long **pp)
{
        return **pp + 1;
}

unsigned long long
unsigned_long_long_read(unsigned long long *p)
{
        return *p;
}

long
unsigned_long_long_long(unsigned long long *p)
{
        return *p;
}

unsigned long
unsigned_long_long_unsigned_long(unsigned long long *p)
{
        return *p;
}

int
unsigned_long_long_int(unsigned long long **pp)
{
        return **pp + 1;
}

enum e
enum_read(enum e *p)
{
        return *p;
}

long
enum_long(enum e *p)
{
        return *p;
}

unsigned long
enum_unsigned_long(enum e *p)
{
        return *p;
}

int
enum_int(enum e **pp)
{
        return **pp + 1;
}
