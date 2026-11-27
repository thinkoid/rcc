/* The width rule, A4 of the survey: a value of int widened to long is
   extended where its register is not valid to 64 bits, movslq %eax,
   %rax and sxtw x0, w0, after the operation that wrote it at 32 bits;
   a truth value, 0 or 1, is valid to 64 already, so (long) (a < b)
   writes none, and a conversion to int caps it at 32, so (long) (int)
   (a < b) writes one.  A remainder is extended from rdx into the rax a
   return names, movslq %edx, %rax.  An int object widened is its
   extending load, movslq -4(%rbp), %rax and ldrsw x0, [x29, #16], read
   under a cast to its own type too. */
long
sum_of_ints(void)
{
        long l = 1;
        int a = 2;
        int b = 3;
        return l + (a + b);
}

long
sum_of_ints_on_left(void)
{
        long l = 1;
        int a = 2;
        int b = 3;
        return (a + b) + l;
}

long
assigned(void)
{
        int c;
        return c = 5;
}

long
compared(void)
{
        int a = 1;
        int b = 2;
        return (long) (a < b);
}

long
compared_as_int(void)
{
        int a = 1;
        int b = 2;
        return (long) (int) (a < b);
}

long
negated(void)
{
        int a = 1;
        return !a;
}

long
logical(void)
{
        int a = 1;
        int b = 2;
        return a && b;
}

long
operations(void)
{
        int a = 1;
        int b = 2;
        return a * 5;
}

long
quotient(void)
{
        int a = 7;
        int b = 2;
        return a / b;
}

long
remainder(void)
{
        int a = 7;
        int b = 2;
        return a % b;
}

long
shift(void)
{
        int a = 7;
        int b = 2;
        return a << b;
}

long
negation(void)
{
        int a = 7;
        return -a;
}

long
complement(void)
{
        int a = 7;
        return ~a;
}

long
object(void)
{
        int a = 7;
        return a;
}

long
object_cast_to_own_type(void)
{
        int a = 7;
        return (long) (int) a;
}

long
narrowed_then_widened(void)
{
        long a = 7;
        return (long) (int) a;
}

long
assignment_widened(void)
{
        int c;
        long a = 7;
        return c = a;
}

int
compared_in_long(void)
{
        long l = 1;
        int i = 2;
        return l < i + 1;
}

int
stored_remainder(void)
{
        long l;
        int a = 7;
        int b = 2;
        l = a % b;
        return 0;
}

long
two_casts(void)
{
        int a = 7;
        int b = 2;
        return (long) (a + b) + (long) (a - b);
}
