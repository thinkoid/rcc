/* E1 op= E2, 6.5.17.3, of every operator on an int: the right operand
   first, then the object's read, the operation and the store, one
   evaluation, p4.  For its effect x86-64 writes + - & | ^ << >> on the
   object in memory, addl $5, -4(%rbp), shll %cl, -4(%rbp), and * / %
   load, operate and store; as a value the object is the source of a
   commuting operation on the right operand's register where nothing
   names another, addl -4(%rbp), %ecx, and is loaded otherwise; as a
   condition + - & | ^ jump on the memory form's flags.  aarch64 loads,
   operates and stores in every form, an immediate where the
   instruction encodes it and otherwise the right operand held first.

   Each function is one operator in one form, its name the two. */

int
add_statement(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        a += b;
        return a;
}

int
add_constant(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        a += 5;
        return a;
}

int
add_expression(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        a += b + c;
        return a;
}

int
add_value(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        return a += b;
}

int
add_value_constant(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        return a += 5;
}

int
add_stored(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        c = a += b;
        return c;
}

int
add_condition(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        if (a += b)
                return 1;
        return 0;
}

int
subtract_statement(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        a -= b;
        return a;
}

int
subtract_constant(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        a -= 5;
        return a;
}

int
subtract_expression(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        a -= b + c;
        return a;
}

int
subtract_value(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        return a -= b;
}

int
subtract_value_constant(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        return a -= 5;
}

int
subtract_stored(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        c = a -= b;
        return c;
}

int
subtract_condition(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        if (a -= b)
                return 1;
        return 0;
}

int
multiply_statement(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        a *= b;
        return a;
}

int
multiply_constant(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        a *= 5;
        return a;
}

int
multiply_expression(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        a *= b + c;
        return a;
}

int
multiply_value(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        return a *= b;
}

int
multiply_value_constant(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        return a *= 5;
}

int
multiply_stored(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        c = a *= b;
        return c;
}

int
multiply_condition(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        if (a *= b)
                return 1;
        return 0;
}

int
divide_statement(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        a /= b;
        return a;
}

int
divide_constant(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        a /= 5;
        return a;
}

int
divide_expression(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        a /= b + c;
        return a;
}

int
divide_value(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        return a /= b;
}

int
divide_value_constant(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        return a /= 5;
}

int
divide_stored(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        c = a /= b;
        return c;
}

int
divide_condition(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        if (a /= b)
                return 1;
        return 0;
}

int
remainder_statement(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        a %= b;
        return a;
}

int
remainder_constant(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        a %= 5;
        return a;
}

int
remainder_expression(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        a %= b + c;
        return a;
}

int
remainder_value(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        return a %= b;
}

int
remainder_value_constant(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        return a %= 5;
}

int
remainder_stored(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        c = a %= b;
        return c;
}

int
remainder_condition(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        if (a %= b)
                return 1;
        return 0;
}

int
shift_left_statement(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        a <<= b;
        return a;
}

int
shift_left_constant(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        a <<= 5;
        return a;
}

int
shift_left_expression(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        a <<= b + c;
        return a;
}

int
shift_left_value(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        return a <<= b;
}

int
shift_left_value_constant(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        return a <<= 5;
}

int
shift_left_stored(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        c = a <<= b;
        return c;
}

int
shift_left_condition(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        if (a <<= b)
                return 1;
        return 0;
}

int
shift_right_statement(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        a >>= b;
        return a;
}

int
shift_right_constant(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        a >>= 5;
        return a;
}

int
shift_right_expression(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        a >>= b + c;
        return a;
}

int
shift_right_value(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        return a >>= b;
}

int
shift_right_value_constant(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        return a >>= 5;
}

int
shift_right_stored(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        c = a >>= b;
        return c;
}

int
shift_right_condition(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        if (a >>= b)
                return 1;
        return 0;
}

int
and_statement(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        a &= b;
        return a;
}

int
and_constant(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        a &= 5;
        return a;
}

int
and_expression(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        a &= b + c;
        return a;
}

int
and_value(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        return a &= b;
}

int
and_value_constant(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        return a &= 5;
}

int
and_stored(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        c = a &= b;
        return c;
}

int
and_condition(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        if (a &= b)
                return 1;
        return 0;
}

int
or_statement(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        a |= b;
        return a;
}

int
or_constant(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        a |= 5;
        return a;
}

int
or_expression(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        a |= b + c;
        return a;
}

int
or_value(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        return a |= b;
}

int
or_value_constant(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        return a |= 5;
}

int
or_stored(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        c = a |= b;
        return c;
}

int
or_condition(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        if (a |= b)
                return 1;
        return 0;
}

int
xor_statement(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        a ^= b;
        return a;
}

int
xor_constant(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        a ^= 5;
        return a;
}

int
xor_expression(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        a ^= b + c;
        return a;
}

int
xor_value(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        return a ^= b;
}

int
xor_value_constant(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        return a ^= 5;
}

int
xor_stored(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        c = a ^= b;
        return c;
}

int
xor_condition(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        if (a ^= b)
                return 1;
        return 0;
}
