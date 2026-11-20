/* cost.md chapter 0 and backend.md 6.4, an identity: an operator whose
   constant operand makes it the identity, + - | ^ << >> by 0, * and /
   by 1, & by every bit of its type, writes no instruction.  Its other
   operand is generated as the operator's type asks, its loads, calls
   and conversions all kept.  A constant on the left of + * & | ^ is the
   same, and so is a constant the parser folded to one, (2 - 2).  What
   does work stays: a * 0, whose value is not a's, & 0xffffffff on a
   long, 0 - a, 0 << a and 1 / a.

   On x86-64 the listing must show add_zero() to slashed_one(),
   masked_unsigned() on an unsigned, the five of a constant on the left
   and spelled() each as movl -4(%rbp), %eax alone after the parameter's
   store, wider_one() as movq -8(%rbp), %rax, promoted() on a signed
   char as movsbl -1(%rbp), %eax and widened() as movslq -4(%rbp), %rax;
   low_bits() as the load, movabsq $4294967295, %rcx and andq %rcx,
   %rax; zero_minus() as subl -4(%rbp), %eax after movl $0, %eax,
   zero_shifted() as shll %cl, %eax, one_over() as cltd and idivl
   -4(%rbp), and times_zero() as imull $0, -4(%rbp), %eax; pointer(),
   pointer_less() and zero_plus_pointer() as movq -8(%rbp), %rax alone;
   row() as the product's addq %rcx, %rax and through() as movq (%rax),
   %rax, with no addq after either; called() as the call with nothing
   after it, loaded() as movl (%rax), %eax, and divided() as cltd and
   idivl -8(%rbp) with no imull.

   On aarch64 the listing must show the same functions each as ldr w0,
   [x29, #16] alone, wider_one() as ldr x0, promoted() as ldrsb w0 and
   widened() as ldrsw x0; low_bits() as and x0, x0, #0xffffffff;
   zero_minus() as sub w0, w0, w1, zero_shifted() as lsl w0, w0, w1 and
   one_over() as sdiv w0, w0, w1, each after mov w0 of the constant, and
   times_zero() as mov w1, #0 and mul w0, w0, w1; the three pointers as
   ldr x0 alone; row() as the product's add x0, x0, x1 and through() as
   ldr x0, [x0], with no add x0, x0, #0 after either; called() as bl f
   with nothing after it, loaded() as ldr w0, [x0], and divided() as
   sdiv w0, w0, w1 with no mul. */
int
add_zero(int a)
{
        return a + 0;
}

int f(int);

int
minus_zero(int a)
{
        return a - 0;
}

int
or_zero(int a)
{
        return a | 0;
}

int
xor_zero(int a)
{
        return a ^ 0;
}

int
shifted_zero(int a)
{
        return a << 0;
}

int
shifted_right_zero(int a)
{
        return a >> 0;
}

int
times_one(int a)
{
        return a * 1;
}

int
and_ones(int a)
{
        return a & -1;
}

int
slashed_one(int a)
{
        return a / 1;
}

long
wider_one(long a)
{
        return a * 1;
}

unsigned
masked_unsigned(unsigned u)
{
        return u & -1;
}

int
promoted(signed char c)
{
        return c + 0;
}

long
widened(int i)
{
        return i + 0L;
}

long
low_bits(long l)
{
        return l & 0xffffffff;
}

int
zero_plus(int a)
{
        return 0 + a;
}

int
one_times(int a)
{
        return 1 * a;
}

int
zero_or(int a)
{
        return 0 | a;
}

int
zero_xor(int a)
{
        return 0 ^ a;
}

int
ones_and(int a)
{
        return -1 & a;
}

int
zero_minus(int a)
{
        return 0 - a;
}

int
zero_shifted(int a)
{
        return 0 << a;
}

int
one_over(int a)
{
        return 1 / a;
}

int
times_zero(int a)
{
        return a * 0;
}

int *
pointer(int *p)
{
        return p + 0;
}

int *
pointer_less(int *p)
{
        return p - 0;
}

int *
zero_plus_pointer(int *p)
{
        return 0 + p;
}

int *
row(int (*p)[3], int i)
{
        return p[i] + 0;
}

int *
through(int **pp)
{
        return &**pp + 0;
}

int
called(int x)
{
        return f(x) + 0;
}

int
loaded(int *p)
{
        return *p + 0;
}

int
divided(int a, int b)
{
        return a / b * 1;
}

int
spelled(int a)
{
        return a + (2 - 2);
}
