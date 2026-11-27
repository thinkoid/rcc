/* A call's result of each type returned from a function of each type:
   the result's conversion to the function's, over the call.  A
   widening of a narrow, an int or an unsigned result writes its
   extension after the call, movsbq %al, %rax and sxtb x0, w0, and
   movl %eax, %eax and mov w0, w0 for an unsigned one, valid to 32
   bits alone; a narrowing and a change of sign write nothing. */
char result_char(void);
signed char result_signed_char(void);
unsigned char result_unsigned_char(void);
short result_short(void);
unsigned short result_unsigned_short(void);
int result_int(void);
unsigned result_unsigned(void);
long result_long(void);
unsigned long result_unsigned_long(void);

char
char_returned_as_char(void)
{
        return result_char();
}

signed char
char_returned_as_signed_char(void)
{
        return result_char();
}

unsigned char
char_returned_as_unsigned_char(void)
{
        return result_char();
}

short
char_returned_as_short(void)
{
        return result_char();
}

unsigned short
char_returned_as_unsigned_short(void)
{
        return result_char();
}

int
char_returned_as_int(void)
{
        return result_char();
}

unsigned
char_returned_as_unsigned(void)
{
        return result_char();
}

long
char_returned_as_long(void)
{
        return result_char();
}

unsigned long
char_returned_as_unsigned_long(void)
{
        return result_char();
}

char
signed_char_returned_as_char(void)
{
        return result_signed_char();
}

signed char
signed_char_returned_as_signed_char(void)
{
        return result_signed_char();
}

unsigned char
signed_char_returned_as_unsigned_char(void)
{
        return result_signed_char();
}

short
signed_char_returned_as_short(void)
{
        return result_signed_char();
}

unsigned short
signed_char_returned_as_unsigned_short(void)
{
        return result_signed_char();
}

int
signed_char_returned_as_int(void)
{
        return result_signed_char();
}

unsigned
signed_char_returned_as_unsigned(void)
{
        return result_signed_char();
}

long
signed_char_returned_as_long(void)
{
        return result_signed_char();
}

unsigned long
signed_char_returned_as_unsigned_long(void)
{
        return result_signed_char();
}

char
unsigned_char_returned_as_char(void)
{
        return result_unsigned_char();
}

signed char
unsigned_char_returned_as_signed_char(void)
{
        return result_unsigned_char();
}

unsigned char
unsigned_char_returned_as_unsigned_char(void)
{
        return result_unsigned_char();
}

short
unsigned_char_returned_as_short(void)
{
        return result_unsigned_char();
}

unsigned short
unsigned_char_returned_as_unsigned_short(void)
{
        return result_unsigned_char();
}

int
unsigned_char_returned_as_int(void)
{
        return result_unsigned_char();
}

unsigned
unsigned_char_returned_as_unsigned(void)
{
        return result_unsigned_char();
}

long
unsigned_char_returned_as_long(void)
{
        return result_unsigned_char();
}

unsigned long
unsigned_char_returned_as_unsigned_long(void)
{
        return result_unsigned_char();
}

char
short_returned_as_char(void)
{
        return result_short();
}

signed char
short_returned_as_signed_char(void)
{
        return result_short();
}

unsigned char
short_returned_as_unsigned_char(void)
{
        return result_short();
}

short
short_returned_as_short(void)
{
        return result_short();
}

unsigned short
short_returned_as_unsigned_short(void)
{
        return result_short();
}

int
short_returned_as_int(void)
{
        return result_short();
}

unsigned
short_returned_as_unsigned(void)
{
        return result_short();
}

long
short_returned_as_long(void)
{
        return result_short();
}

unsigned long
short_returned_as_unsigned_long(void)
{
        return result_short();
}

char
unsigned_short_returned_as_char(void)
{
        return result_unsigned_short();
}

signed char
unsigned_short_returned_as_signed_char(void)
{
        return result_unsigned_short();
}

unsigned char
unsigned_short_returned_as_unsigned_char(void)
{
        return result_unsigned_short();
}

short
unsigned_short_returned_as_short(void)
{
        return result_unsigned_short();
}

unsigned short
unsigned_short_returned_as_unsigned_short(void)
{
        return result_unsigned_short();
}

int
unsigned_short_returned_as_int(void)
{
        return result_unsigned_short();
}

unsigned
unsigned_short_returned_as_unsigned(void)
{
        return result_unsigned_short();
}

long
unsigned_short_returned_as_long(void)
{
        return result_unsigned_short();
}

unsigned long
unsigned_short_returned_as_unsigned_long(void)
{
        return result_unsigned_short();
}

char
int_returned_as_char(void)
{
        return result_int();
}

signed char
int_returned_as_signed_char(void)
{
        return result_int();
}

unsigned char
int_returned_as_unsigned_char(void)
{
        return result_int();
}

short
int_returned_as_short(void)
{
        return result_int();
}

unsigned short
int_returned_as_unsigned_short(void)
{
        return result_int();
}

int
int_returned_as_int(void)
{
        return result_int();
}

unsigned
int_returned_as_unsigned(void)
{
        return result_int();
}

long
int_returned_as_long(void)
{
        return result_int();
}

unsigned long
int_returned_as_unsigned_long(void)
{
        return result_int();
}

char
unsigned_returned_as_char(void)
{
        return result_unsigned();
}

signed char
unsigned_returned_as_signed_char(void)
{
        return result_unsigned();
}

unsigned char
unsigned_returned_as_unsigned_char(void)
{
        return result_unsigned();
}

short
unsigned_returned_as_short(void)
{
        return result_unsigned();
}

unsigned short
unsigned_returned_as_unsigned_short(void)
{
        return result_unsigned();
}

int
unsigned_returned_as_int(void)
{
        return result_unsigned();
}

unsigned
unsigned_returned_as_unsigned(void)
{
        return result_unsigned();
}

long
unsigned_returned_as_long(void)
{
        return result_unsigned();
}

unsigned long
unsigned_returned_as_unsigned_long(void)
{
        return result_unsigned();
}

char
long_returned_as_char(void)
{
        return result_long();
}

signed char
long_returned_as_signed_char(void)
{
        return result_long();
}

unsigned char
long_returned_as_unsigned_char(void)
{
        return result_long();
}

short
long_returned_as_short(void)
{
        return result_long();
}

unsigned short
long_returned_as_unsigned_short(void)
{
        return result_long();
}

int
long_returned_as_int(void)
{
        return result_long();
}

unsigned
long_returned_as_unsigned(void)
{
        return result_long();
}

long
long_returned_as_long(void)
{
        return result_long();
}

unsigned long
long_returned_as_unsigned_long(void)
{
        return result_long();
}

char
unsigned_long_returned_as_char(void)
{
        return result_unsigned_long();
}

signed char
unsigned_long_returned_as_signed_char(void)
{
        return result_unsigned_long();
}

unsigned char
unsigned_long_returned_as_unsigned_char(void)
{
        return result_unsigned_long();
}

short
unsigned_long_returned_as_short(void)
{
        return result_unsigned_long();
}

unsigned short
unsigned_long_returned_as_unsigned_short(void)
{
        return result_unsigned_long();
}

int
unsigned_long_returned_as_int(void)
{
        return result_unsigned_long();
}

unsigned
unsigned_long_returned_as_unsigned(void)
{
        return result_unsigned_long();
}

long
unsigned_long_returned_as_long(void)
{
        return result_unsigned_long();
}

unsigned long
unsigned_long_returned_as_unsigned_long(void)
{
        return result_unsigned_long();
}
