/* A parameter of each integer type read and written: its slot of its
   type's size and alignment, stored from its argument register at its
   width, movb %dil, -1(%rbp) and strb w0, [x29, #16]; each read the
   extending load of its type from the slot, movsbl -1(%rbp), %eax, and
   each write the store at its width, movb %al, -1(%rbp). */
char
read_char(char a)
{
        return a;
}

signed char
read_signed_char(signed char a)
{
        return a;
}

unsigned char
read_unsigned_char(unsigned char a)
{
        return a;
}

short
read_short(short a)
{
        return a;
}

unsigned short
read_unsigned_short(unsigned short a)
{
        return a;
}

int
read_int(int a)
{
        return a;
}

long
read_long(long a)
{
        return a;
}

char
incremented_char(char a)
{
        a = a + 1;
        return a;
}

signed char
incremented_signed_char(signed char a)
{
        a = a + 1;
        return a;
}

unsigned char
incremented_unsigned_char(unsigned char a)
{
        a = a + 1;
        return a;
}

short
incremented_short(short a)
{
        a = a + 1;
        return a;
}

unsigned short
incremented_unsigned_short(unsigned short a)
{
        a = a + 1;
        return a;
}

int
incremented_int(int a)
{
        a = a + 1;
        return a;
}

long
incremented_long(long a)
{
        a = a + 1;
        return a;
}
