/* !, && and || as jumps, 6.5.14p4, 6.5.15p4: ! writes nothing and
   flips the sense of its operand's jump; && jumping on 0 and || on
   nonzero jump to the target from each operand, with no label of
   their own; && jumping on nonzero and || on 0 jump from the left
   operand past the right one, to a label taken before the left one and
   placed after the right one's jump.  An if jumps on 0, so a top &&
   takes no label and a top || one: disjunction()'s if (a || b) is
   jne .Ldisjunction.3, je .Ldisjunction.2 and .Ldisjunction.3:.
   Labels are numbered in the order the walk takes them and placed
   where written: if (a && b || c) takes 2 for the if, 3 for the || and
   4 for the && inside it, and places 4, 3, 2.  A value of && inside a
   condition is a value: if ((a && b) + c) takes 2 for the if, then 3
   and 4 for the value, its je after the addl.

   Each function is one if over one condition, its name the
   condition's form. */
int
negation(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (!a)
                return 1;
        return 0;
}

int
double_negation(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (!!a)
                return 1;
        return 0;
}

int
negated_comparison(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (!(a < b))
                return 1;
        return 0;
}

int
conjunction(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a && b)
                return 1;
        return 0;
}

int
disjunction(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a || b)
                return 1;
        return 0;
}

int
negated_conjunction(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (!(a && b))
                return 1;
        return 0;
}

int
negated_disjunction(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (!(a || b))
                return 1;
        return 0;
}

int
conjunction_of_three(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a && b && c)
                return 1;
        return 0;
}

int
disjunction_of_three(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a || b || c)
                return 1;
        return 0;
}

int
and_or(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a && b || c)
                return 1;
        return 0;
}

int
or_and(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a || b && c)
                return 1;
        return 0;
}

int
and_parenthesized_or(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a && (b || c))
                return 1;
        return 0;
}

int
parenthesized_or_and(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if ((a || b) && c)
                return 1;
        return 0;
}

int
comparisons_and(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a < b && b < c)
                return 1;
        return 0;
}

int
comparisons_or(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a < b || b < c)
                return 1;
        return 0;
}

int
conjunction_value(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if ((a && b) + c)
                return 1;
        return 0;
}

int
zero_and(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (0 && a)
                return 1;
        return 0;
}

int
one_or(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (1 || a)
                return 1;
        return 0;
}

int
and_one(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a && 1)
                return 1;
        return 0;
}

int
not_zero(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (!0)
                return 1;
        return 0;
}

int
not_one(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (!1)
                return 1;
        return 0;
}
