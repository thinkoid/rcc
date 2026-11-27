/* An if whose then part is one jump, a break or a continue, bare or the
   one item of a block, is the condition's jump to the jump's label
   where the condition is nonzero, and nothing else: no label of the
   if's own and no jump.  An else part follows the condition's jump
   with no jump over it.  if (b && c) break; takes a label for its &&,
   which jumps on nonzero.  A block whose one item is a block, and a
   block of two items, are not one jump, though they jump: the then
   part is written as any, a jump over a jmp.

   Each function is one loop around one such if, its name the if's
   form. */
int
break_on_local(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        while (a) {
                if (b)
                        break;
        }
        return 0;
}

int
break_on_negation(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        while (a) {
                if (!b)
                        break;
        }
        return 0;
}

int
break_on_comparison(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        while (a) {
                if (b < c)
                        break;
        }
        return 0;
}

int
break_on_conjunction(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        while (a) {
                if (b && c)
                        break;
        }
        return 0;
}

int
break_on_disjunction(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        while (a) {
                if (b || c)
                        break;
        }
        return 0;
}

int
continue_on_conjunction(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        while (a) {
                if (b && c)
                        continue;
                a = 0;
        }
        return 0;
}

int
continue_in_for(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        for (a = 0; a < 10; a = a + 1) {
                if (b)
                        continue;
                c = c + 1;
        }
        return c;
}

int
break_in_block(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        while (a) {
                if (b) {
                        break;
                }
        }
        return 0;
}

int
break_else(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        while (a) {
                if (b)
                        break;
                else
                        a = 0;
        }
        return 0;
}

int
break_else_return(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        while (a) {
                if (b)
                        break;
                else
                        return 3;
        }
        return 0;
}

int
break_else_break(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        while (a) {
                if (b)
                        break;
                else
                        break;
        }
        return 0;
}

int
continue_else_continue(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        while (a) {
                if (b)
                        continue;
                else
                        continue;
        }
        return 0;
}

int
continue_else_block(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        for (a = 0; a < 3; a = a + 1) {
                if (b)
                        continue;
                else {
                        c = 1;
                        break;
                }
        }
        return c;
}

int
block_in_block(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        while (a) {
                if (b) {
                        {
                                break;
                        }
                }
        }
        return 0;
}

int
two_items(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        while (a) {
                if (b) {
                        ;
                        break;
                }
        }
        return 0;
}

int
break_in_do(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        do {
                if (a)
                        break;
        } while (b);
        return 0;
}
