/* return; in a void function's loops and branches: an if of one jump
   in a loop's body, the return as a body of its own, and in an else
   part, where the else part's jump is the return's own. */
int g(int a);

void
loops(int a)
{
        while (a) {
                if (g(a))
                        return;
                a = a - 1;
        }
        for (;;)
                return;
}

void
branches(int a)
{
        if (a)
                a = g(a);
        else
                return;
        if (g(a))
                return;
        else
                a = 2;
}
