/* ++ and --, 6.5.4.1, 6.5.3.5: the object stepped by 1 and stored, the
   value the new one, prefix, or the old one, postfix; no rewriting as
   E += 1.  For its effect x86-64 writes the step on the object in
   memory, addl $1, -4(%rbp), never incl, and as a condition a prefix
   step jumps on that instruction's flags; aarch64 loads, adds and
   stores.  A postfix value on x86-64 is a copy of the old value, taken
   before the add, in a register outside the one the consumer names;
   on aarch64 the old value stays where its load put it and the new one
   takes a fresh register.

   Each function is one shape, its name the shape. */
int g(int x);

int
postfix_statement(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        a++;
        return a;
}

int
postfix_decrement_statement(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        a--;
        return a;
}

int
prefix_statement(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        ++a;
        return a;
}

int
prefix_decrement_statement(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        --a;
        return a;
}

int
parenthesized(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        (a)++;
        return a;
}

int
prefix_returned(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return ++a;
}

int
prefix_decrement_returned(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return --a;
}

int
postfix_returned(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return a++;
}

int
postfix_decrement_returned(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return a--;
}

int
postfix_stored(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        c = a++;
        return c;
}

int
postfix_decrement_stored(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        c = a--;
        return c;
}

int
prefix_stored(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        c = ++a;
        return c;
}

int
prefix_decrement_stored(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        c = --a;
        return c;
}

int
postfix_initializer(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        int d = a++;
        return d;
}

int
prefix_initializer(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        int d = ++a;
        return d;
}

int
left_of_sum(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return a++ + b;
}

int
right_of_sum(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return b + a++;
}

int
prefix_right_of_sum(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return b + ++a;
}

int
right_of_sum_stored(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        c = b + a++;
        return c;
}

int
right_of_difference(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return b - a--;
}

int
two(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return a++ + b++;
}

int
same_twice(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return a++ + a++;
}

int
after_product(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return a * b + c++;
}

int
negated(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return -a++;
}

int
not(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return !a--;
}

int
compared(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return a++ < b;
}

int
product(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return a++ * 3;
}

int
divisor(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return b / a++;
}

int
lexed(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return a++ + b;
}

int
condition_prefix(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (++a)
                return 1;
        return 0;
}

int
condition_prefix_decrement(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (--a)
                return 1;
        return 0;
}

int
condition_postfix(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a++)
                return 1;
        return 0;
}

int
condition_postfix_decrement(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a--)
                return 1;
        return 0;
}

int
condition_compared(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a++ < b)
                return 1;
        return 0;
}

int
condition_prefix_equal_to_zero(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (++a == 0)
                return 1;
        return 0;
}

int
condition_postfix_equal_to_zero(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a++ == 0)
                return 1;
        return 0;
}

int
while_prefix(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        while (--a)
                b = b + 1;
        return b;
}

int
for_step(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        for (a = 0; a < 10; a++)
                b = b + a;
        return b;
}

int
for_prefix_step(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        for (a = 0; a < 10; ++a)
                ;
        return a;
}

int
comma_of_steps(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return (a++, b);
}

int
comma_steps_statement(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        a++, b--;
        return a;
}

int
count(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return b << a++;
}

int
count_prefix(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return b << ++a;
}

int
argument(void)
{
        int a = 1;
        return g(a++);
}

int
argument_prefix(void)
{
        int a = 1;
        return g(++a);
}

int
parameter(int p)
{
        p++;
        return --p;
}

int
for_declaration(void)
{
        int a = 0;
        for (int i = 0; i < 3; i++)
                a = a + i;
        return a;
}

int
held_across_call(void)
{
        int a = 1;
        int b = 2;
        return a++ + g(b++);
}
