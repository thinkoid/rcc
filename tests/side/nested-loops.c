/* Loops in loops, and the labels each takes at its own moment of the
   one walk in source order: a while takes the head's and the one past
   it on reaching it; a do the body's, the test's and the one past it;
   a for the head's, the step's and the one past it after its init; an
   if the one past its then part before its condition, and the one past
   its else part after its then part.  A label is placed where its
   statement writes it, and one no jump names is not placed: a do's
   test label without a continue, its label past the loop without a
   break, a for's step label without a continue, its label past the
   loop with no condition and no break.  A break and a continue jump to
   the innermost loop's labels, whatever lies between.

   for_while_if() places 2, 5, 7, 8, 6, 4 and never 3;
   logical_while() takes 4 for the || of its test and 5 to 7 for the
   value in its body; break_inner() breaks out of a for in a while,
   then out of the while; three_loops() is a while, a do and a for in
   sequence; continue_inner() continues a do in a for, then the for;
   one_jump_chain() is an if of one jump whose else part is another;
   break_through_if() breaks out of loops in both parts of an if;
   loops_in_if() puts a loop in each part of an if; ending_jumps() is a
   body whose two parts end in a break and a continue. */
int
for_while_if(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        for (a = 0; a < 10; a = a + 1) {
                while (b) {
                        if (c)
                                b = b - 1;
                        else
                                c = 1;
                }
        }
        return c;
}

int
logical_while(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        while (a && (b || c)) {
                c = a || b && c;
        }
        return c;
}

int
break_inner(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        while (a) {
                for (;;) {
                        if (b)
                                break;
                        c = c + 1;
                }
                if (c)
                        break;
                a = a - 1;
        }
        return c;
}

int
three_loops(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        while (a)
                a = a - 1;
        do
                b = b - 1;
        while (b);
        for (c = 0; c < 3; c = c + 1)
                ;
        return c;
}

int
continue_inner(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        for (a = 0; a < 3; a = a + 1) {
                do {
                        if (b)
                                continue;
                        c = 1;
                } while (c);
                if (c)
                        continue;
                b = 1;
        }
        return b;
}

int
one_jump_chain(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        for (a = 0; a < 10; a = a + 1)
                while (b)
                        if (c && a)
                                break;
                        else if (c || b)
                                continue;
                        else
                                b = 0;
        return 0;
}

int
break_through_if(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        for (;;) {
                if (a) {
                        while (b) {
                                b = b - 1;
                                if (!b)
                                        break;
                        }
                        if (c)
                                break;
                }
                else {
                        do {
                                if (a || b)
                                        break;
                        } while (c);
                        return 2;
                }
        }
        return 1;
}

int
loops_in_if(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a) {
                while (b < 3) {
                        b = b + 1;
                        if (b == 2)
                                continue;
                        c = !c;
                }
        }
        else {
                for (c = 0; c < 2; c = c + 1)
                        if (a && b)
                                a = 0;
        }
        return c;
}

int
ending_jumps(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        while (a) {
                if (b) {
                        c = 1;
                        break;
                }
                else {
                        c = 2;
                        continue;
                }
        }
        return c;
}
