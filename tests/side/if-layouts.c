/* The layouts of if and if else, 6.8.5.2: the label past the then
   part is taken on reaching the if, before the condition's own, and
   the condition jumps to it where it is 0.  With an else, a then part
   that a path leaves at its end takes a jump past the else part, and
   the label past it is taken then, after every label of the then part;
   a then part that ends in a jump takes neither, and falls into
   nothing.  An empty then part, ; or { }, changes nothing; a constant
   condition is the code of any condition.  An else goes with the
   nearest if.  if (a) b = 1; else if (c) b = 2; else b = 3; takes 2,
   then 3 after the then part, then 4 and 5 for the inner if, and places
   2, 4, 5, 3; an inner if without an else places its label after its
   then part's jump, so the outer then part does not end in a jump and
   takes its label past the else part.

   Each function is one if, its name its layout. */
int
if_alone(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a)
                b = 1;
        return b;
}

int
if_else(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a)
                b = 1;
        else
                b = 2;
        return b;
}

int
if_block(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a) {
                b = 1;
                c = 2;
        }
        return b;
}

int
then_returns(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a) {
                b = 1;
                return b;
        }
        else
                b = 2;
        return b;
}

int
else_returns(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a)
                b = 1;
        else
                return 2;
        return b;
}

int
both_return(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a)
                return 1;
        else
                return 2;
}

int
empty_then(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a)
                ;
        return 0;
}

int
empty_block_then(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a) {
        }
        else
                b = 2;
        return b;
}

int
empty_else(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a)
                b = 1;
        else
                ;
        return b;
}

int
constant_zero(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (0)
                b = 1;
        else
                b = 2;
        return b;
}

int
constant_one(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (1)
                b = 1;
        else
                b = 2;
        return b;
}

int
else_if(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a)
                b = 1;
        else if (c)
                b = 2;
        else
                b = 3;
        return b;
}

int
nested(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a) {
                if (b)
                        c = 1;
                else
                        c = 2;
        }
        else {
                if (c)
                        b = 1;
                else
                        b = 2;
        }
        return c;
}

int
nested_returns(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a) {
                if (b)
                        return 1;
                else
                        return 2;
        }
        else
                c = 3;
        return c;
}

int
nested_without_else(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a) {
                if (b)
                        return 1;
        }
        else
                c = 3;
        return c;
}

int
dangling_else(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a)
                if (b)
                        c = 1;
                else
                        c = 2;
        return c;
}

int
logical_else(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a || b)
                c = 1;
        else
                c = 2;
        return c;
}
