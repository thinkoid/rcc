/* Values held across a call.  At the call's start every held value is
   spilled, in register order, rax, rcx, ... and x0, x1, ..., not by
   age: each to a fresh slot of 8 bytes at the next multiple of 8, movq
   %rax, -24(%rbp) and str x0, [x29, #32], after a hole of 4 bytes where
   an odd count of int slots precedes it.  The call's own slots come
   after the spill's.  An argument's slot is taken before its value, so
   a spill inside an argument lands after it.  A call that is the left
   operand, or a constant left operand of an operator that commutes,
   which is taken on the right, holds nothing; a constant left operand
   of - is a held value.  A call's result is a held value at the next
   call.  Calls as arguments hold nothing: each result is stored to its
   slot before the next argument. */
int g(int a);
int h(int a);
int k(int a, int b);

int
one(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return a + g(b);
}

int
two(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return a + (b + g(c));
}

int
three(void)
{
        int a = 1;
        return a + (a + (a + g(a)));
}

int
register_order(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return a + b / g(c);
}

int
results(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return g(a) * g(b) + g(c);
}

int
none_held(void)
{
        int a = 1;
        int b = 2;
        return g(a) + b + (5 + g(a));
}

int
constant_left(void)
{
        int a = 1;
        return 5 - g(a);
}

int
arguments(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return k(g(a), h(b)) + k(a, b + g(c)) + k(a + g(b), c);
}
