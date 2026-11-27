/* && and || as values, 6.5.14p3, 6.5.15p3: jump code into one register
   on both paths, with nothing held across them.  The labels of the 0
   path and of the join are taken first, then the jump code's own:
   return a || b; takes 2 and 3, then 4 for the ||, and places 4, 2, 3.
   The register is taken after the jump code, movl $1 and movl $0 on
   x86-64, mov w0, #1 and mov w0, #0 on aarch64, never set from the
   flags.  Two constants fold, 2 && 3 to 1 and 0 || 0 to 0; a constant
   beside a local does not, so 0 && a is the jump code, the constant
   loaded and tested.  ! of a value of && is the test of its register.
   As a statement the value is computed and dropped, with its labels.

   returned_*() return one value each, its name the form;
   assigned_conjunction() stores one, and discarded() drops three. */
int
returned_conjunction(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return a && b;
}

int
returned_disjunction(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return a || b;
}

int
returned_and_or(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return a && b || c;
}

int
returned_or_and(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return a || b && c;
}

int
returned_conjunction_of_three(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return a && b && c;
}

int
returned_values(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return (a && b) && (b || c);
}

int
returned_comparisons(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return a < b && b < c;
}

int
returned_negated(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return !(a && b);
}

int
returned_sum(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return (a && b) + c;
}

int
returned_folded_and(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return 2 && 3;
}

int
returned_folded_or(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return 0 || 0;
}

int
returned_zero_and(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return 0 && a;
}

int
returned_one_and(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return 1 && a;
}

int
returned_and_one(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return a && 1;
}

int
returned_one_or(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return 1 || a;
}

int
returned_or_zero(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return a || 0;
}

int
assigned_conjunction(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        c = a && b;
        return c;
}

int
discarded(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        a &&b;
        a || b;
        !a;
        return 0;
}
