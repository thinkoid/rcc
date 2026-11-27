/* The conditions on the narrow types, C7 of the survey.  On x86-64 an
   object alone is compared in place at its width, cmpb $0, -1(%rbp),
   and so is one under a conversion that keeps its bits, if ((unsigned
   char) c); but a == 0 compares the promoted value, its load and cmpl
   $0, %eax.  On aarch64 a value valid to less than 32 bits, the value of
   a narrowing or of an assignment to a narrow object, is tested by tst
   w0, #0xff or #0xffff, and any other by cbz: if (c = a) on an int a is
   tst, if (c = d) on a char d is cbz, d's load being valid to 32; and
   an operand of && or || is tested at its own type, so (unsigned char)
   c && x is tst where c && x is cbz. */
int
alone(void)
{
        char c = 1;
        short s = 2;
        if (c)
                return 1;
        if (s)
                return 2;
        return 0;
}

int
equal_zero(void)
{
        char a = 1;
        if (a == 0)
                return 1;
        return 0;
}

int
assigned_int(void)
{
        char c;
        int a = 1;
        if (c = a)
                return 1;
        return 0;
}

int
assigned_char(void)
{
        char c;
        char d = 1;
        if (c = d)
                return 1;
        return 0;
}

int
assigned_short(void)
{
        unsigned short c;
        long a = 1;
        while (c = a)
                a = a - 1;
        return 0;
}

int
change_of_sign(void)
{
        signed char c = 1;
        if ((unsigned char) c)
                return 1;
        return 0;
}

int
narrowed(void)
{
        int a = 1;
        if ((char) a)
                return 1;
        if ((short) (a + 1))
                return 2;
        if (!(unsigned char) a)
                return 3;
        return 0;
}

int
negated(void)
{
        char c = 1;
        short s = 2;
        return !c + !s + !(char) s;
}

int
logical(void)
{
        char c = 1;
        unsigned short s = 2;
        if (c && s || !c)
                return 1;
        return 0;
}

int
loops(void)
{
        unsigned char c = 10;
        int n = 0;
        while (c)
                c = c - 1;
        do
                n = n + 1;
        while (c = n);
        return n;
}

int
logical_cast(void)
{
        unsigned char c = 1;
        int x = 2;
        return ((unsigned char) c && x) + (c && x) + ((char) c || x);
}
