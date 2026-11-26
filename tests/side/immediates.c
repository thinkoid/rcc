/* The constants each target takes as an instruction's immediate, at
   the edges of what it encodes.  x86-64 takes every int as the
   immediate of add, sub, and, or and xor, negative ones as written.
   aarch64's add and sub take 0 to 4095, or that shifted left by 12, of
   either sign, written signed with the source's operator; and, orr and
   eor take a bitmask, a rotated run of ones repeated, written in hex,
   and never 0 or -1.  A constant neither instruction encodes is moved
   into a register of its own after the left operand, by one mov or a
   mov and a movk, and the operation takes the register.

   Each function returns a local and one constant under one operator,
   its name the operator and the constant; left_100000() and
   left_sub_100000() put the constant on the left, where + takes it on
   the right and - into the register first. */
int
add_minus_one(void)
{
        int a = 7;
        return a + -1;
}

int
sub_minus_one(void)
{
        int a = 7;
        return a - -1;
}

int
add_4095(void)
{
        int a = 7;
        return a + 4095;
}

int
add_4096(void)
{
        int a = 7;
        return a + 4096;
}

int
add_4097(void)
{
        int a = 7;
        return a + 4097;
}

int
add_minus_4095(void)
{
        int a = 7;
        return a + -4095;
}

int
add_minus_4096(void)
{
        int a = 7;
        return a + -4096;
}

int
sub_4096(void)
{
        int a = 7;
        return a - 4096;
}

int
add_shifted(void)
{
        int a = 7;
        return a + 16773120;
}

int
sub_shifted(void)
{
        int a = 7;
        return a - 16773120;
}

int
add_minus_shifted(void)
{
        int a = 7;
        return a + -16773120;
}

int
add_past_shifted(void)
{
        int a = 7;
        return a + 16773121;
}

int
add_16777216(void)
{
        int a = 7;
        return a + 16777216;
}

int
add_minus_past_shifted(void)
{
        int a = 7;
        return a + -16773121;
}

int
add_100000(void)
{
        int a = 7;
        return a + 100000;
}

int
add_max(void)
{
        int a = 7;
        return a + 2147483647;
}

int
sub_max(void)
{
        int a = 7;
        return a - 2147483647;
}

int
add_min(void)
{
        int a = 7;
        return a + (-2147483647 - 1);
}

int
and_3(void)
{
        int a = 7;
        return a & 3;
}

int
and_255(void)
{
        int a = 7;
        return a & 255;
}

int
and_240(void)
{
        int a = 7;
        return a & 240;
}

int
and_5(void)
{
        int a = 7;
        return a & 5;
}

int
and_minus_two(void)
{
        int a = 7;
        return a & -2;
}

int
and_alternate(void)
{
        int a = 7;
        return a & 1431655765;
}

int
or_1(void)
{
        int a = 7;
        return a | 1;
}

int
or_minus_two(void)
{
        int a = 7;
        return a | -2;
}

int
or_5(void)
{
        int a = 7;
        return a | 5;
}

int
xor_minus_two(void)
{
        int a = 7;
        return a ^ -2;
}

int
xor_65535(void)
{
        int a = 7;
        return a ^ 65535;
}

int
xor_100000(void)
{
        int a = 7;
        return a ^ 100000;
}

int
xor_5(void)
{
        int a = 7;
        return a ^ 5;
}

int
or_minus_one(void)
{
        int a = 7;
        return a | -1;
}

int
xor_minus_one(void)
{
        int a = 7;
        return a ^ -1;
}

int
left_100000(void)
{
        int a = 7;
        return 100000 + a;
}

int
left_sub_100000(void)
{
        int a = 7;
        return 100000 - a;
}
