/* Two arguments of each pair of widths: the slots taken after the two
   locals, each of its parameter's size and alignment, in order. */
int two_char_char(char x, char y);
int two_char_short(char x, short y);
int two_char_int(char x, int y);
int two_char_long(char x, long y);
int two_short_char(short x, char y);
int two_short_short(short x, short y);
int two_short_int(short x, int y);
int two_short_long(short x, long y);
int two_int_char(int x, char y);
int two_int_short(int x, short y);
int two_int_int(int x, int y);
int two_int_long(int x, long y);
int two_long_char(long x, char y);
int two_long_short(long x, short y);
int two_long_int(long x, int y);
int two_long_long(long x, long y);

int
pass_char_char(void)
{
        char a;
        char b;
        return two_char_char(a, b);
}

int
pass_char_short(void)
{
        char a;
        short b;
        return two_char_short(a, b);
}

int
pass_char_int(void)
{
        char a;
        int b;
        return two_char_int(a, b);
}

int
pass_char_long(void)
{
        char a;
        long b;
        return two_char_long(a, b);
}

int
pass_short_char(void)
{
        short a;
        char b;
        return two_short_char(a, b);
}

int
pass_short_short(void)
{
        short a;
        short b;
        return two_short_short(a, b);
}

int
pass_short_int(void)
{
        short a;
        int b;
        return two_short_int(a, b);
}

int
pass_short_long(void)
{
        short a;
        long b;
        return two_short_long(a, b);
}

int
pass_int_char(void)
{
        int a;
        char b;
        return two_int_char(a, b);
}

int
pass_int_short(void)
{
        int a;
        short b;
        return two_int_short(a, b);
}

int
pass_int_int(void)
{
        int a;
        int b;
        return two_int_int(a, b);
}

int
pass_int_long(void)
{
        int a;
        long b;
        return two_int_long(a, b);
}

int
pass_long_char(void)
{
        long a;
        char b;
        return two_long_char(a, b);
}

int
pass_long_short(void)
{
        long a;
        short b;
        return two_long_short(a, b);
}

int
pass_long_int(void)
{
        long a;
        int b;
        return two_long_int(a, b);
}

int
pass_long_long(void)
{
        long a;
        long b;
        return two_long_long(a, b);
}
