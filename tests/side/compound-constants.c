/* The constants of a compound assignment: nothing folds it and no
   identity drops it, so a += 0 adds 0 and a *= 1 multiplies by 1, as
   in rcc; an immediate is written as the instruction encodes it, on
   aarch64 12 bits or 12 shifted for add and sub, a bitmask for and, orr
   and eor, a shift's count masked to the width, and otherwise held
   first, mov w0, #4097, while x86-64 writes the source's constant.

   Each function is one constant, its name the constant. */

int
add_zero(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        a += 0;
        return a;
}

int
subtract_zero(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        a -= 0;
        return a;
}

int
multiply_one(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        a *= 1;
        return a;
}

int
multiply_zero(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        a *= 0;
        return a;
}

int
divide_one(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        a /= 1;
        return a;
}

int
remainder_one(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        a %= 1;
        return a;
}

int
and_zero(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        a &= 0;
        return a;
}

int
and_all(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        a &= -1;
        return a;
}

int
or_zero(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        a |= 0;
        return a;
}

int
xor_zero(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        a ^= 0;
        return a;
}

int
shift_zero(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        a <<= 0;
        return a;
}

int
shift_32(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        a >>= 32;
        return a;
}

int
add_zero_value(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        return a += 0;
}

int
add_4095(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        a += 4095;
        return a;
}

int
add_4096(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        a += 4096;
        return a;
}

int
add_4097(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        a += 4097;
        return a;
}

int
add_big(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        a += 100000;
        return a;
}

int
add_minus_one(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        a += -1;
        return a;
}

int
subtract_minus_4095(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        a -= -4095;
        return a;
}

int
and_255(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        a &= 255;
        return a;
}

int
or_big(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        a |= 100000;
        return a;
}

int
multiply_12(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        a *= 12;
        return a;
}

int
multiply_12_value(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        return a *= 12;
}

int
multiply_big(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        a *= 100000;
        return a;
}

int
divide_zero(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        a /= 0;
        return a;
}

int
divide_minus_one(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        a /= -1;
        return a;
}

int
remainder_7_value(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        return a %= 7;
}

int
shift_33(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        a <<= 33;
        return a;
}

int
shift_right_31(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        a >>= 31;
        return a;
}
