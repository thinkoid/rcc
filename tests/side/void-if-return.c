/* In a void function, if (c) return; is an if of one jump, as if (c)
   break; is: the condition's jump to the epilogue's label where it is
   nonzero, cmpl $0, -4(%rbp) and jne .Lf.1, cbnz w0, .Lf.1, and no
   label of the if's own; an else part follows with no jump over it.
   The return as the one item of a block is one jump too; in a block in
   a block it is not, and the if writes its own label. */
void
middle(void)
{
        int a = 1;
        if (a)
                return;
        a = 2;
}

void
otherwise(void)
{
        int a = 1;
        if (a)
                return;
        else
                a = 2;
        a = 3;
}

void
braced(void)
{
        int a = 1;
        if (a) {
                return;
        }
        a = 2;
}

void
nested(void)
{
        int a = 1;
        if (a) {
                {
                        return;
                }
        }
        a = 2;
}

void
in_loop(void)
{
        int a = 1;
        while (a) {
                if (a)
                        return;
                a = a - 1;
        }
}
