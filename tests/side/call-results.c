/* A call's result is held in rax or x0, where the call leaves it, and
   its consumer takes it there: a return writes only its jump, a store
   stores eax or w0, an operator works on it in place, an initializer's
   slot is taken before the call's arguments'.  A discarded result
   writes nothing after the call, but for an operator on it, which is
   written; a void call as a statement is the call alone.  A call as an
   argument is stored from the result register to the outer slot. */
int g(int a);
void v(int a);

int
returned(int a)
{
        return g(a);
}

int
stored(int a)
{
        a = g(a);
        return a;
}

int
stored_returned(int a, int b)
{
        return a = g(b);
}

int
chained(int a, int b, int c)
{
        a = b = g(c);
        return a;
}

int
initialized(int a)
{
        int x = g(a);
        return x;
}

int
operators(int a)
{
        int x = g(a) + 1;
        int y = g(a) * 3;
        int z = -g(a);
        int w = ~g(a);
        return x + y + z + w + g(a) / 2;
}

int
discarded(int a)
{
        g(a);
        g(a) + 1;
        v(a);
        return g(g(a));
}

void
void_last(int a)
{
        v(a);
}
