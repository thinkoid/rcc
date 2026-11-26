/* Blocks and the scopes they open, 6.2.1p4.  A block's locals take
   slots of the one frame, which nothing gives back at the block's end,
   G12: two sibling blocks do not share a slot, and a declaration after
   a block takes the next one.  An inner declaration hides an outer one
   of the same name to the block's end, and a use of a name finds the
   innermost declaration in scope.

   shadow() returns the inner x; siblings() declares y and z in two
   blocks, and w after them; outer() reads the outer x inside a block
   that declares a y; nested() nests blocks three deep; empty() holds an
   empty block, which writes nothing; named() declares a local named as
   its function, which hides the function in its body. */
int
shadow(void)
{
        int x = 7;
        {
                int x = 8;
                return x;
        }
}

int
siblings(void)
{
        int x = 1;
        {
                int y = 2;
        }
        {
                int z = 3;
        }
        int w = x;
        return w;
}

int
outer(void)
{
        int x = 4;
        {
                int y = x;
                return y;
        }
}

int
nested(void)
{
        int a = 1;
        {
                int b = a;
                {
                        int a = b;
                        {
                                return a;
                        }
                }
        }
}

int
empty(void)
{
        {
        }
        return 0;
}

int
named(void)
{
        int named = 9;
        return named;
}
