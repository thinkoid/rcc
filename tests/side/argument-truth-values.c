/* A truth value as an argument of each type, which is valid to 64
   bits and needs no extension to a long, and the caller's own
   parameter of each type passed on as an argument of its type. */
int takes_char(char x);
int takes_signed_char(signed char x);
int takes_unsigned_char(unsigned char x);
int takes_short(short x);
int takes_unsigned_short(unsigned short x);
int takes_int(int x);
int takes_long(long x);

int
truth_to_char(void)
{
        int a;
        int b;
        return takes_char(a < b);
}

int
truth_to_signed_char(void)
{
        int a;
        int b;
        return takes_signed_char(a < b);
}

int
truth_to_unsigned_char(void)
{
        int a;
        int b;
        return takes_unsigned_char(a < b);
}

int
truth_to_short(void)
{
        int a;
        int b;
        return takes_short(a < b);
}

int
truth_to_unsigned_short(void)
{
        int a;
        int b;
        return takes_unsigned_short(a < b);
}

int
truth_to_int(void)
{
        int a;
        int b;
        return takes_int(a < b);
}

int
truth_to_long(void)
{
        int a;
        int b;
        return takes_long(a < b);
}

int
parameter_to_char(char a)
{
        return takes_char(a);
}

int
parameter_to_signed_char(signed char a)
{
        return takes_signed_char(a);
}

int
parameter_to_unsigned_char(unsigned char a)
{
        return takes_unsigned_char(a);
}

int
parameter_to_short(short a)
{
        return takes_short(a);
}

int
parameter_to_unsigned_short(unsigned short a)
{
        return takes_unsigned_short(a);
}

int
parameter_to_int(int a)
{
        return takes_int(a);
}

int
parameter_to_long(long a)
{
        return takes_long(a);
}
