/* A narrow object under a change of sign and a widening, read at the
   type converted from as a value, (int) (unsigned short) s on a short s
   being movzwl, ldrh, and at its own type where a ?: jumps on it, rcc's
   under_widening(), which reads through both conversions: ldrsh on
   aarch64; x86-64 compares it in place, cmpw $0.  The same of a signed
   char under unsigned char, and of an unsigned char under signed char,
   as an arm, as the condition, and under a promotion. */
static unsigned char gu;
extern short gs;

int
arm_short(void)
{
        int a;
        int b;
        int c;
        short s;
        if (a ? b < c : (int) (unsigned short) s)
                return 1;
        return 0;
}

int
arm_char(void)
{
        int a;
        int b;
        int c;
        signed char s;
        if (a ? b < c : (int) (unsigned char) s)
                return 1;
        return 0;
}

int
arm_unsigned_char(void)
{
        int a;
        int b;
        int c;
        unsigned char u;
        if (a ? (int) (signed char) u : b < c)
                return 1;
        return 0;
}

int
condition(void)
{
        int b;
        int c;
        unsigned char u;
        if ((int) (signed char) u ? b < c : c)
                return 1;
        return 0;
}

int
condition_promoted(void)
{
        int b;
        int c;
        unsigned char u;
        if ((long) (int) (signed char) u ? b < c : c)
                return 1;
        return 0;
}

int
values(void)
{
        short s;
        return (int) (unsigned short) s;
}

long
values_promoted(void)
{
        unsigned char u;
        return (long) (int) (signed char) u;
}

unsigned long
values_unsigned(void)
{
        signed char c;
        return (unsigned long) (unsigned) (unsigned char) c;
}

int
objects(void)
{
        int a;
        int b;
        int c;
        if (a ? (int) (signed char) gu : b < c)
                return 1;
        if (a ? b < c : (int) (unsigned short) gs)
                return 2;
        return 0;
}
