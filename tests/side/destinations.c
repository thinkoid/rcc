/* The destinations of an operator's value: the operator's row returns
   its value in a register it takes, and each destination reads it
   there.  A return finds it in the result register, the first one a
   statement takes; x = a + b; and int x = a + b; store it from its
   register; a + b; computes it and drops it; return x = a + b; stores
   it and leaves it in the result register; x = a = a + b; stores the
   inner assignment first.  x = x + 5; is the load, the add and the
   store, since no compound assignment is written.

   returned(), assigned(), initialized(), discarded(), returned_store()
   and chained() are each destination of a + b; incremented() and
   accumulated() add a constant and a local to x itself; negated()
   assigns a negation; folded() initializes from a fold to 0, which
   is a constant's store, and assigns another.  The destinations of a
   product are the same rows: product_assigned(),
   product_initialized(), product_discarded() and
   product_returned_store() each take a * b or a * 5, and scaled()
   multiplies x by a constant, imull $5, -8(%rbp), %eax, then the
   store.  So are a shift's: shift_assigned(), shift_initialized(),
   shift_discarded() and shift_returned_store() each take a << b or
   a >> 5, and shifted() and shifted_by() shift x by a constant and by
   a local, the count in ecx, then the store. */
int
returned(void)
{
        int a = 1;
        int b = 2;
        return a + b;
}

int
assigned(void)
{
        int a = 1;
        int b = 2;
        int x;
        x = a + b;
        return x;
}

int
initialized(void)
{
        int a = 1;
        int b = 2;
        int x = a - b;
        return x;
}

int
discarded(void)
{
        int a = 1;
        int b = 2;
        a & b;
        a | 5;
        return 0;
}

int
returned_store(void)
{
        int a = 1;
        int b = 2;
        int x;
        return x = a ^ b;
}

int
chained(void)
{
        int a = 1;
        int b = 2;
        int x;
        x = a = a + b;
        return x;
}

int
incremented(void)
{
        int x = 1;
        x = x + 5;
        return x;
}

int
accumulated(void)
{
        int b = 2;
        int x = 1;
        x = x + b;
        return x;
}

int
negated(void)
{
        int a = 1;
        int x;
        x = -a;
        return x;
}

int
folded(void)
{
        int x = 1 - 1;
        x = 3 - 3;
        return x;
}

int
product_assigned(void)
{
        int a = 1;
        int b = 2;
        int x;
        x = a * b;
        return x;
}

int
product_initialized(void)
{
        int a = 1;
        int b = 2;
        int x = a * 5;
        return x;
}

int
product_discarded(void)
{
        int a = 1;
        int b = 2;
        (a * b);
        a * 5;
        return 0;
}

int
product_returned_store(void)
{
        int a = 1;
        int b = 2;
        int x;
        return x = a * 5;
}

int
scaled(void)
{
        int b = 2;
        int x = 1;
        x = x * 5;
        return x;
}

int
shift_assigned(void)
{
        int a = 1;
        int b = 2;
        int x;
        x = a << b;
        return x;
}

int
shift_initialized(void)
{
        int a = 1;
        int b = 2;
        int x = a >> 5;
        return x;
}

int
shift_discarded(void)
{
        int a = 1;
        int b = 2;
        a << b;
        a >> 5;
        return 0;
}

int
shift_returned_store(void)
{
        int a = 1;
        int b = 2;
        int x;
        return x = a << b;
}

int
shifted(void)
{
        int b = 2;
        int x = 1;
        x = x << 5;
        return x;
}

int
shifted_by(void)
{
        int b = 2;
        int x = 1;
        x = x << b;
        return x;
}
