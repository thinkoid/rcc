/* Divisions under and over the other operators.  On x86-64 idivl pins
   eax and edx: the left operand of a node whose right operand is a
   division is loaded away from them, so a + (b / a) holds a in ecx and
   divides b in eax, then adds ecx into eax, the quotient's register,
   since + commutes and eax is the register a return names; a - (b / a)
   subtracts into ecx and moves it to eax.  A value held where a
   division writes moves out of the way first: (a / b) + (a / b) moves
   the first quotient to esi, movq %rax, %rsi.  A shift's count goes
   into cl, so a / (b << a) shifts b in esi and divides by it, and
   a << (b / a) holds a in esi and moves the quotient to ecx.  A
   dividend that lands in edx moves to eax for its quotient:
   a + (b + (b / a)) holds a in eax and b in ecx, loads the dividend
   into edx, moves a to esi and the dividend to eax.  A division inside
   an assignment on the right steers nothing, a + (x = b / a), but
   moves the value held in eax out of its way.  A shift's count names
   ecx, and the name goes down a division's dividend as a return's eax
   does: a remainder and a product under & as a count end andl %edx,
   %ecx, and a sum divided as a count adds into ecx, addl %eax, %ecx,
   before it moves to eax for idivl.  A value the shift needs out of
   ecx moves, its own shifted value too: (c % a) >> (a / b) has the
   remainder moved to ecx by the second division, movq %rdx, %rcx,
   then back out of the count's way, movq %rcx, %rdx.  aarch64 pins
   nothing, and every shape is that of +.

   Each function returns one such expression, its name what it
   shows. */
int
add_div(void)
{
        int a = 3;
        int b = 50;
        return a + (b / a);
}

int
sub_div(void)
{
        int a = 3;
        int b = 50;
        return a - (b / a);
}

int
quotients_added(void)
{
        int a = 50;
        int b = 3;
        return (a / b) + (a / b);
}

int
divided_by_shift(void)
{
        int a = 3;
        int b = 5;
        return a / (b << a);
}

int
shifted_by_quotient(void)
{
        int a = 3;
        int b = 50;
        return a << (b / a);
}

int
remainder_by_shift(void)
{
        int a = 3;
        int b = 5;
        return a % (b << a);
}

int
shift_divided(void)
{
        int a = 50;
        int b = 3;
        return (a << b) / a;
}

int
sum_divided(void)
{
        int a = 50;
        int b = 3;
        return (a + b) / 5;
}

int
sum_remainder(void)
{
        int a = 50;
        int b = 3;
        return (a + b) % 5;
}

int
divided_twice(void)
{
        int a = 500;
        int b = 3;
        return a / b / 5;
}

int
remainders_multiplied(void)
{
        int a = 50;
        int b = 7;
        return (a % b) * (a % b);
}

int
mul_remainder(void)
{
        int a = 3;
        int b = 50;
        return a * (b % a);
}

int
constant_add_div(void)
{
        int a = 3;
        int b = 50;
        return 7 + (b / a);
}

int
constant_sub_div(void)
{
        int a = 3;
        return 70 - (a / a);
}

int
quotient_times_constant(void)
{
        int a = 50;
        int b = 3;
        return (a / b) * 5;
}

int
negated_quotient(void)
{
        int a = 3;
        int b = 50;
        return -(b / (a / a));
}

int
remainder_by_quotient(void)
{
        int a = 50;
        int b = 7;
        return a % (b / (a % b));
}

int
or_divided_by_and(void)
{
        int a = 3;
        int b = 50;
        return (a | b) / (b & a);
}

int
dividend_in_rdx(void)
{
        int a = 3;
        int b = 50;
        return a + (b + (b / a));
}

int
add_assigned_quotient(void)
{
        int a = 3;
        int b = 50;
        int x;
        return a + (x = b / a);
}

int
divided_by_assigned(void)
{
        int a = 3;
        int b = 50;
        int x;
        return b / (x = b - a);
}

int
count_and_of_remainder(void)
{
        int a = 3;
        int b = 5;
        int d = 11;
        return (+b << (b << d)) >> ((a % a) & (b * d));
}

int
count_quotient(void)
{
        int a = 3;
        int c = 7;
        int d = 11;
        return (((a + d) ^ (d - d)) << (d = a)) << ((2 * c + (c ^ a)) / -d);
}

int
remainder_shifted_by_quotient(void)
{
        int a = 3;
        int b = 5;
        int c = 7;
        return (c % a) >> (a / b);
}
