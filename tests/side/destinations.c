/* The destinations of an operator's value: the operator writes its
   value into the register its parent hands it, so each destination is
   the row it already has.  A return computes it in the result
   register; x = a + b; and int x = a + b; in the register a value
   takes, then store it; a + b; computes it and drops it; return
   x = a + b; stores it and leaves it in the result register; x = a =
   a + b; stores the inner assignment first.  x = x + 5; is the load,
   the add and the store, since no compound assignment is written.

   returned(), assigned(), initialized(), discarded(), returned_store()
   and chained() are each destination of a + b; incremented() and
   accumulated() add a constant and a local to x itself; negated()
   assigns a negation; folded() initializes from a fold to 0, which
   is a constant's store, and assigns another. */
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
