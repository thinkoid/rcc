/* The comma, 6.5.18p2: its left operand evaluated for its effect, then
   its right one by the entry point the comma was called by, for its
   value, its jump or its effect, and nothing of its own.  So a = 1, b;
   stores 1 in place before b's value; (1, 2) loads both constants, a
   comma folding nothing, 6.6p3; c = (1, 2); stores from a register,
   since an immediate is stored in place only where it is the value
   itself; and if (a = 1, b) jumps as if (b) does, cmpl $0, -8(%rbp) on
   x86-64.

   Each function is one position, its name the position. */
int k(int x, int y);
int g(int x);

int
returned(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return (a, b);
}

int
returned_unparenthesized(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return a = 1, b;
}

int
constants(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return (1, 2);
}

int
constants_stored(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        c = (1, 2);
        return c;
}

int
zero_stored(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        c = (a, 0);
        return c;
}

int
statement(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        a = 1, b = 2;
        return a;
}

int
bare(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        (a, b);
        return 0;
}

int
three(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return (a = 1, b = 2, a + b);
}

int
nested(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return (a, (b, c));
}

int
left_of_sum(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return (a, b) + c;
}

int
right_of_sum(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return a + (b, c);
}

int
right_of_sum_assigned(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return a + (b = 1, c);
}

int
right_of_sum_stored(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        c = a + (b = 1, c);
        return c;
}

int
right_of_sum_held(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return a + (b = 2, b + c);
}

int
remainder_stored(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        c = (a, b % c);
        return c;
}

int
remainder_returned(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return (a, b % c);
}

int
stored(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        c = (a, b);
        return c;
}

int
stored_assignment(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        c = (a = 1, b + 1);
        return c;
}

int
argument(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return k((a, b), c);
}

int
call_left(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return (g(a), b);
}

int
condition(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a = 1, b)
                return 1;
        return 0;
}

int
condition_comparison(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a = 2, b < c)
                return 1;
        return 0;
}

int
condition_negated(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (!(a = 2, b))
                return 1;
        return 0;
}

int
condition_conjunction(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if ((a = 2, b) && c)
                return 1;
        return 0;
}

int
condition_flags(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a = 1, b + c)
                return 1;
        return 0;
}

int
while_condition(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        while (a = a - 1, a)
                b = b + 1;
        return b;
}

int
do_condition(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        do
                b = b + 1;
        while (a = 0, b < 3);
        return b;
}

int
for_clauses(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        for (a = 0, b = 0; a < 10; a = a + 1, b = b + 2)
                c = c + 1;
        return b;
}

int
for_condition(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        for (; a = a - 1, a;)
                b = b + 1;
        return b;
}
