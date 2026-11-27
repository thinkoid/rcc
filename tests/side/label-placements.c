/* Where a label may stand, 6.8.2: before a statement, as a block item
   of its own before a declaration and before the closing brace of a
   block or of the body, 6.8.3p1.  A label never used draws nothing; a
   label's name is in a name space of its own, 6.2.3p1, so a local may
   bear it; and a goto may jump past a declaration with an initializer
   and into a block past one, which draws nothing either.

   Each function is one shape, its name the shape. */
int
unused(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
L:
        a = 1;
        return a;
}

int
two_unused(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
L:
M:
        a = 1;
        return a;
}

int
named_as_local(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
a:
        a = 1;
        goto a;
}

int
before_block_end(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        {
                a = 1;
        L:
        }
        return a;
}

int
before_body_end(void)
{
        int a = 1;
        goto L;
        a = 1;
L:
}

int
before_declaration(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        goto L;
L:
        int d = 1;
        return d;
}

int
past_declaration(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        goto L;
        int d = 5;
L:
        return d;
}

int
into_block_past_declaration(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        goto L;
        {
                int d = 5;
        L:
                a = d;
        }
        return a;
}
