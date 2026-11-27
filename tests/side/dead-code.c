/* Code after a jump is generated all the same, and an if there starts
   with no path reaching it: a dead if else whose then part places no
   label still has no path out of it at its end, so it writes no jump
   over its else part and takes no label past it, and the then part
   falls into the else part.  The label past the then part is placed,
   and a path reaches what follows it.

   after_return() and after_both_return() put an if, and a store, after
   a return; if_else_after_return() and empty_then_after_return() put
   an if else there. */
int
after_return(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return 1;
        if (a)
                b = 1;
        return b;
}

int
after_both_return(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a)
                return 1;
        else
                return 2;
        b = 3;
        return b;
}

int
if_else_after_return(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return 1;
        if (a)
                b = 1;
        else
                b = 2;
        return b;
}

int
empty_then_after_return(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return 1;
        if (a)
                ;
        else
                b = 2;
        return b;
}
