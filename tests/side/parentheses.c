/* A parenthesized expression, 6.5.2p6, is the expression inside, the
   same node: nothing marks it.  So (x) = 1; stores into x, and a
   parenthesis around a local, a constant or another parenthesis writes
   nothing of its own.

   assigned() stores through parenthesized names, (x) = 1; and
   ((x)) = (y);, and returns ((x) = 2); returned() returns a name in
   two parentheses; initialized() initializes from a name and from
   constants in parentheses, 0 among them; discarded() evaluates
   parenthesized expressions for their effect. */
int
assigned(void)
{
        int x;
        int y = 4;
        (x) = 1;
        ((x)) = (y);
        return ((x) = 2);
}

int
returned(void)
{
        int x = 3;
        return ((x));
}

int
initialized(void)
{
        int x = (5);
        int y = ((0));
        int z = (x);
        return (7);
}

int
discarded(void)
{
        int x = 3;
        (x);
        ((7));
        (x = 4);
        return x;
}
