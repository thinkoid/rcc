/* An if whose then part is one goto, bare or the one item of a block,
   is the condition's jump to the goto's label where the condition is
   nonzero, and nothing else, as for a break: no label of the if's own
   and no jump.  An else part follows the condition's jump.

   Each function is one if (C) goto L;, its name the condition. */
int
on_local(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a)
                goto L;
        b = 1;
L:
        return b;
}

int
in_block(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a) {
                goto L;
        }
        b = 1;
L:
        return b;
}

int
on_comparison(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a < b)
                goto L;
        c = 1;
L:
        return c;
}

int
on_conjunction(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a && b)
                goto L;
        c = 1;
L:
        return c;
}

int
on_disjunction(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a || b)
                goto L;
        c = 1;
L:
        return c;
}

int
backward(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        a = 0;
top:
        a = a + 1;
        if (a < 10)
                goto top;
        return a;
}

int
with_else(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a)
                goto L;
        else
                b = 1;
        c = 2;
L:
        return b;
}

long
on_long(void)
{
        long a = 4294967296;
        if (a)
                goto L;
        a = 1;
L:
        return a;
}

int
on_char(void)
{
        char a = 'a';
        if (a == 'b')
                goto L;
        a = 1;
L:
        return a;
}
