/* A widening over a widening of an object, rcc's widened_read(): a
   widening reads the object in one load where its operand is the object
   under casts that keep its bits, or one promotion of the object, a
   signed one under a signed widening or any unsigned one, (long) (int)
   x on a signed char x being movsbq -1(%rbp), %rax.  A widening over a
   widening over the object is over a value, extended by the width
   rule: (long) (int) (short) x is movsbl -1(%rbp), %eax and movslq
   %eax, %rax.  Each as a value, an operand, a condition and an
   argument. */
int takes_long(long a);

long
promoted_twice(void)
{
        signed char x = 1;
        return (long) (int) (short) x;
}

long
promoted_then_returned(void)
{
        signed char x = 1;
        return (int) (short) x;
}

long
promoted_once(void)
{
        signed char x = 1;
        return (long) (short) x;
}

long
promotion_widened(void)
{
        signed char x = 1;
        return (long) (int) x;
}

long
unsigned_promoted_twice(void)
{
        unsigned char x = 1;
        return (long) (int) (unsigned short) x;
}

long
unsigned_promoted_then_returned(void)
{
        unsigned char x = 1;
        return (int) (unsigned short) x;
}

long
unsigned_promoted_once(void)
{
        unsigned char x = 1;
        return (long) (unsigned short) x;
}

long
unsigned_promotion_widened(void)
{
        unsigned char x = 1;
        return (long) (int) x;
}

unsigned long
unsigned_all_the_way(void)
{
        unsigned char x = 1;
        return (unsigned long) (unsigned) (unsigned short) x;
}

long
signed_under_unsigned(void)
{
        signed char x = 1;
        return (long) (unsigned) (short) x;
}

unsigned long
signed_promoted_to_unsigned_long(void)
{
        signed char x = 1;
        return (unsigned long) (int) (short) x;
}

long
unsigned_char_of_signed(void)
{
        signed char x = 1;
        return (long) (int) (unsigned char) x;
}

long
unsigned_of_unsigned_char(void)
{
        signed char x = 1;
        return (long) (unsigned) (unsigned char) x;
}

long
short_of_unsigned_char(void)
{
        unsigned char x = 1;
        return (long) (int) (short) x;
}

long
short_promoted_twice(void)
{
        short x = 1;
        return (long) (int) x;
}

long
short_unsigned_twice(void)
{
        unsigned short x = 1;
        return (long) (unsigned) x;
}

long
kept_bits_then_twice(void)
{
        char x = 1;
        return (long) (int) (short) (signed char) x;
}

long
kept_bits_unsigned_then_twice(void)
{
        short x = 1;
        return (long) (int) (unsigned short) (short) x;
}

long
kept_bits_of_unsigned_char(void)
{
        unsigned char x = 1;
        return (long) (int) (short) (unsigned char) x;
}

long
promoted_twice_plus(void)
{
        signed char x = 1;
        long l = 2;
        return (long) (int) (short) x + l;
}

int
promoted_twice_compared(void)
{
        signed char x = 1;
        long l = 2;
        return (long) (int) (short) x < l;
}

int
promoted_twice_tested(void)
{
        signed char x = 1;
        if ((long) (int) (short) x)
                return 1;
        return 0;
}

int
promoted_twice_passed(void)
{
        signed char x = 1;
        return takes_long((long) (int) (short) x);
}

long
promoted_then_returned_plus(void)
{
        signed char x = 1;
        long l = 2;
        return (int) (short) x + l;
}

int
promoted_then_returned_compared(void)
{
        signed char x = 1;
        long l = 2;
        return (int) (short) x < l;
}

int
promoted_then_returned_tested(void)
{
        signed char x = 1;
        if ((int) (short) x)
                return 1;
        return 0;
}

int
promoted_then_returned_passed(void)
{
        signed char x = 1;
        return takes_long((int) (short) x);
}

long
promoted_once_plus(void)
{
        signed char x = 1;
        long l = 2;
        return (long) (short) x + l;
}

int
promoted_once_compared(void)
{
        signed char x = 1;
        long l = 2;
        return (long) (short) x < l;
}

int
promoted_once_tested(void)
{
        signed char x = 1;
        if ((long) (short) x)
                return 1;
        return 0;
}

int
promoted_once_passed(void)
{
        signed char x = 1;
        return takes_long((long) (short) x);
}

long
promotion_widened_plus(void)
{
        signed char x = 1;
        long l = 2;
        return (long) (int) x + l;
}

int
promotion_widened_compared(void)
{
        signed char x = 1;
        long l = 2;
        return (long) (int) x < l;
}

int
promotion_widened_tested(void)
{
        signed char x = 1;
        if ((long) (int) x)
                return 1;
        return 0;
}

int
promotion_widened_passed(void)
{
        signed char x = 1;
        return takes_long((long) (int) x);
}

long
unsigned_promoted_twice_plus(void)
{
        unsigned char x = 1;
        long l = 2;
        return (long) (int) (unsigned short) x + l;
}

int
unsigned_promoted_twice_compared(void)
{
        unsigned char x = 1;
        long l = 2;
        return (long) (int) (unsigned short) x < l;
}

int
unsigned_promoted_twice_tested(void)
{
        unsigned char x = 1;
        if ((long) (int) (unsigned short) x)
                return 1;
        return 0;
}

int
unsigned_promoted_twice_passed(void)
{
        unsigned char x = 1;
        return takes_long((long) (int) (unsigned short) x);
}

long
unsigned_promoted_then_returned_plus(void)
{
        unsigned char x = 1;
        long l = 2;
        return (int) (unsigned short) x + l;
}

int
unsigned_promoted_then_returned_compared(void)
{
        unsigned char x = 1;
        long l = 2;
        return (int) (unsigned short) x < l;
}

int
unsigned_promoted_then_returned_tested(void)
{
        unsigned char x = 1;
        if ((int) (unsigned short) x)
                return 1;
        return 0;
}

int
unsigned_promoted_then_returned_passed(void)
{
        unsigned char x = 1;
        return takes_long((int) (unsigned short) x);
}

long
signed_under_unsigned_plus(void)
{
        signed char x = 1;
        long l = 2;
        return (long) (unsigned) (short) x + l;
}

int
signed_under_unsigned_compared(void)
{
        signed char x = 1;
        long l = 2;
        return (long) (unsigned) (short) x < l;
}

int
signed_under_unsigned_tested(void)
{
        signed char x = 1;
        if ((long) (unsigned) (short) x)
                return 1;
        return 0;
}

int
signed_under_unsigned_passed(void)
{
        signed char x = 1;
        return takes_long((long) (unsigned) (short) x);
}

long
signed_promoted_to_unsigned_long_plus(void)
{
        signed char x = 1;
        long l = 2;
        return (unsigned long) (int) (short) x + l;
}

int
signed_promoted_to_unsigned_long_compared(void)
{
        signed char x = 1;
        long l = 2;
        return (unsigned long) (int) (short) x < l;
}

int
signed_promoted_to_unsigned_long_tested(void)
{
        signed char x = 1;
        if ((unsigned long) (int) (short) x)
                return 1;
        return 0;
}

int
signed_promoted_to_unsigned_long_passed(void)
{
        signed char x = 1;
        return takes_long((unsigned long) (int) (short) x);
}

long
unsigned_char_of_signed_plus(void)
{
        signed char x = 1;
        long l = 2;
        return (long) (int) (unsigned char) x + l;
}

int
unsigned_char_of_signed_compared(void)
{
        signed char x = 1;
        long l = 2;
        return (long) (int) (unsigned char) x < l;
}

int
unsigned_char_of_signed_tested(void)
{
        signed char x = 1;
        if ((long) (int) (unsigned char) x)
                return 1;
        return 0;
}

int
unsigned_char_of_signed_passed(void)
{
        signed char x = 1;
        return takes_long((long) (int) (unsigned char) x);
}
